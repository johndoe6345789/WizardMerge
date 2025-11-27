"""Extract image XObjects from wizardmerge.pdf and emit a JSON manifest.

The script avoids external dependencies so it can run in constrained environments.
Flate-encoded images are converted into PNG byte streams, while DCT-encoded
images are treated as JPEG. A companion ``images.json`` file captures every
image's metadata, a lightweight content analysis, and a base64 payload without
writing raw binaries to disk. Semantic file names are generated from the
analysis (color, contrast, orientation) so the manifest is easier to navigate.
"""
from __future__ import annotations

import base64
import json
import pathlib
import re
import struct
import zlib
from dataclasses import dataclass
from typing import Dict, Iterable, List, Optional, Tuple

PDF_PATH = pathlib.Path("wizardmerge.pdf")
OUTPUT_DIR = pathlib.Path("extracted_graphics")


@dataclass
class ImageObject:
    """Metadata and raw bytes for a single PDF image object."""

    object_number: int
    width: int
    height: int
    color_space: str
    bits_per_component: int
    filter: str
    stream: bytes

    @property
    def channels(self) -> int:
        if "/DeviceRGB" in self.color_space:
            return 3
        if "/DeviceGray" in self.color_space:
            return 1
        raise ValueError(f"Unsupported colorspace {self.color_space!r}")


OBJECT_PATTERN = re.compile(rb"(\d+)\s+\d+\s+obj(.*?)endobj", re.DOTALL)


def _extract_stream(obj_bytes: bytes) -> bytes:
    """Return the raw stream bytes for a PDF object."""

    stream_match = re.search(rb"stream\r?\n", obj_bytes)
    if not stream_match:
        raise ValueError("No stream found in object")

    start = stream_match.end()
    length_match = re.search(rb"/Length\s+(\d+)", obj_bytes)
    if length_match:
        length = int(length_match.group(1))
        return obj_bytes[start : start + length]

    end = obj_bytes.find(b"endstream", start)
    return obj_bytes[start:end].rstrip(b"\r\n")


def iter_image_objects(pdf_bytes: bytes) -> Iterable[ImageObject]:
    """Yield image objects discovered in the PDF payload."""

    for match in OBJECT_PATTERN.finditer(pdf_bytes):
        obj_bytes = match.group(0)
        if b"/Subtype /Image" not in obj_bytes:
            continue

        object_number = int(match.group(1))

        def _lookup(name: bytes) -> Optional[str]:
            pattern = re.search(rb"/" + name + rb"\s+(/[^\s]+)", obj_bytes)
            return pattern.group(1).decode("ascii") if pattern else None

        width_match = re.search(rb"/Width\s+(\d+)", obj_bytes)
        height_match = re.search(rb"/Height\s+(\d+)", obj_bytes)
        bits_match = re.search(rb"/BitsPerComponent\s+(\d+)", obj_bytes)

        if not (width_match and height_match and bits_match):
            raise ValueError(f"Image {object_number} missing dimension metadata")

        image = ImageObject(
            object_number=object_number,
            width=int(width_match.group(1)),
            height=int(height_match.group(1)),
            color_space=_lookup(b"ColorSpace") or "/DeviceRGB",
            bits_per_component=int(bits_match.group(1)),
            filter=_lookup(b"Filter") or "",
            stream=_extract_stream(obj_bytes),
        )
        yield image


def _png_chunk(tag: bytes, payload: bytes) -> bytes:
    length = struct.pack(">I", len(payload))
    crc = struct.pack(">I", zlib.crc32(tag + payload) & 0xFFFFFFFF)
    return length + tag + payload + crc


def _dominant_color_label(means: Tuple[float, ...]) -> str:
    """Return a coarse color label from per-channel means."""

    if len(means) == 1:
        gray = means[0]
        if gray < 16:
            return "black"
        if gray < 64:
            return "dark-gray"
        if gray < 160:
            return "mid-gray"
        if gray < 224:
            return "light-gray"
        return "white"

    red, green, blue = means
    brightness = (red + green + blue) / 3
    if max(red, green, blue) - min(red, green, blue) < 12:
        # Essentially grayscale
        return _dominant_color_label((brightness,))

    dominant_channel = max(range(3), key=lambda idx: (red, green, blue)[idx])
    channel_names = {0: "red", 1: "green", 2: "blue"}
    brightness_label = _dominant_color_label((brightness,))
    return f"{brightness_label}-{channel_names[dominant_channel]}"


def _orientation_tag(width: int, height: int) -> str:
    if width == height:
        return "square"
    if width > height:
        return "landscape"
    return "portrait"


def analyse_flate_image(image: ImageObject) -> Dict[str, object]:
    """Compute basic color statistics for a Flate-decoded image."""

    raw = zlib.decompress(image.stream)
    row_stride = image.width * image.channels
    expected_size = row_stride * image.height
    if len(raw) != expected_size:
        raise ValueError(
            f"Unexpected data length for image {image.object_number}: "
            f"got {len(raw)}, expected {expected_size}"
        )

    channel_stats = [
        {"count": 0, "mean": 0.0, "m2": 0.0, "min": 255, "max": 0}
        for _ in range(image.channels)
    ]
    palette: set[Tuple[int, ...]] = set()
    palette_limit = 1024

    for idx in range(0, len(raw), image.channels):
        for channel in range(image.channels):
            value = raw[idx + channel]
            stats = channel_stats[channel]
            stats["count"] += 1
            delta = value - stats["mean"]
            stats["mean"] += delta / stats["count"]
            stats["m2"] += delta * (value - stats["mean"])
            stats["min"] = min(stats["min"], value)
            stats["max"] = max(stats["max"], value)

        if len(palette) < palette_limit:
            if image.channels == 1:
                palette.add((raw[idx],))
            else:
                palette.add(tuple(raw[idx : idx + image.channels]))

    means = tuple(stat["mean"] for stat in channel_stats)
    variances = tuple(stat["m2"] / max(stat["count"], 1) for stat in channel_stats)
    palette_size = len(palette) if len(palette) < palette_limit else None
    primary_color = _dominant_color_label(means)

    return {
        "means": means,
        "variances": variances,
        "min": tuple(stat["min"] for stat in channel_stats),
        "max": tuple(stat["max"] for stat in channel_stats),
        "palette_size": palette_size,
        "primary_color": primary_color,
        "orientation": _orientation_tag(image.width, image.height),
    }


def semantic_name(image: ImageObject, mime: str, analysis: Optional[Dict[str, object]]) -> str:
    """Generate a more meaningful file name based on image analysis."""

    extension = "png" if mime == "image/png" else "jpg"
    base_parts = []

    if analysis:
        palette_size = analysis.get("palette_size")
        variances: Tuple[float, ...] = analysis.get("variances", ())  # type: ignore[assignment]
        variance_score = sum(variances) / max(len(variances), 1)
        primary_color = analysis.get("primary_color") or "unknown"
        base_parts.append(primary_color)

        if palette_size == 1:
            base_parts.append("solid")
        elif palette_size and palette_size <= 4:
            base_parts.append("two-tone")
        elif variance_score < 400:
            base_parts.append("low-contrast")
        else:
            base_parts.append("detailed")

        base_parts.append(str(analysis.get("orientation", "unknown")))
    else:
        base_parts.extend(["jpeg", _orientation_tag(image.width, image.height)])

    base_parts.append(f"{image.width}x{image.height}")
    base_parts.append(f"obj{image.object_number}")
    return "-".join(base_parts) + f".{extension}"


def raw_to_png(image: ImageObject) -> tuple[bytes, Dict[str, object]]:
    """Convert a Flate-encoded image stream to PNG bytes and analysis."""

    if image.bits_per_component != 8:
        raise ValueError(f"Unsupported bit depth: {image.bits_per_component}")

    analysis = analyse_flate_image(image)

    raw = zlib.decompress(image.stream)
    row_stride = image.width * image.channels
    filtered = b"".join(
        b"\x00" + raw[i : i + row_stride] for i in range(0, len(raw), row_stride)
    )

    color_type = 2 if image.channels == 3 else 0
    ihdr = struct.pack(
        ">IIBBBBB", image.width, image.height, 8, color_type, 0, 0, 0
    )
    png = b"\x89PNG\r\n\x1a\n"
    png += _png_chunk(b"IHDR", ihdr)
    png += _png_chunk(b"IDAT", zlib.compress(filtered))
    png += _png_chunk(b"IEND", b"")
    return png, analysis


def save_images(images: List[ImageObject]) -> None:
    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)
    manifest: List[dict[str, object]] = []
    errors: List[str] = []

    for index, image in enumerate(sorted(images, key=lambda im: im.object_number), start=1):
        analysis: Optional[Dict[str, object]] = None

        try:
            if image.filter == "/FlateDecode":
                raw_bytes, analysis = raw_to_png(image)
                mime = "image/png"
            elif image.filter == "/DCTDecode":
                raw_bytes = image.stream
                mime = "image/jpeg"
            else:
                raise ValueError(f"Unsupported filter {image.filter}")
        except Exception as exc:  # noqa: BLE001 - surface helpful error context
            placeholder = f"obj{image.object_number}"
            errors.append(f"{placeholder}: {exc}")
            print(f"Skipping {placeholder}: {exc}")
            continue

        name = semantic_name(image, mime, analysis)
        encoded = base64.b64encode(raw_bytes).decode("ascii")
        manifest.append(
            {
                "name": name,
                "object_number": image.object_number,
                "width": image.width,
                "height": image.height,
                "color_space": image.color_space,
                "bits_per_component": image.bits_per_component,
                "mime": mime,
                "base64": encoded,
                "analysis": analysis,
            }
        )
        print(f"Captured {name} ({image.width}x{image.height}, {mime})")

    images_path = OUTPUT_DIR / "images.json"
    images_path.write_text(json.dumps(manifest, indent=2))
    if errors:
        (OUTPUT_DIR / "errors.txt").write_text("\n".join(errors))
        print(f"Encountered errors for {len(errors)} image(s); see errors.txt")
    print(f"Wrote JSON manifest to {images_path}")


def main() -> None:
    pdf_bytes = PDF_PATH.read_bytes()
    images = list(iter_image_objects(pdf_bytes))
    save_images(images)


if __name__ == "__main__":
    main()
