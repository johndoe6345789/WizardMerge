#!/usr/bin/env python3
"""Extract text from page images using OCR and save as a markdown document."""

from pathlib import Path
import pytesseract
from PIL import Image

def ocr_pages(pages_dir: Path, output_file: Path) -> None:
    """Perform OCR on all page images and create a single document."""
    
    pages_dir = pages_dir.resolve()
    if not pages_dir.exists():
        raise FileNotFoundError(f"Pages directory not found: {pages_dir}")
    
    # Get all PNG files sorted by number
    image_files = sorted(
        pages_dir.glob("*.png"),
        key=lambda p: int(p.stem.split("_")[-1])
    )
    
    if not image_files:
        raise ValueError(f"No PNG files found in {pages_dir}")
    
    print(f"Found {len(image_files)} page images to process...")
    
    full_text = []
    full_text.append("# WizardMerge Research Paper\n")
    full_text.append("*Extracted via OCR from paper pages*\n\n")
    full_text.append("---\n\n")
    
    for idx, image_file in enumerate(image_files, start=1):
        print(f"Processing page {idx}/{len(image_files)}: {image_file.name}")
        
        try:
            # Open image and perform OCR
            img = Image.open(image_file)
            text = pytesseract.image_to_string(img)
            
            # Add page separator and text
            full_text.append(f"## Page {idx}\n\n")
            full_text.append(text.strip())
            full_text.append("\n\n---\n\n")
            
        except Exception as e:
            print(f"  Error processing {image_file.name}: {e}")
            full_text.append(f"## Page {idx}\n\n")
            full_text.append(f"*[OCR Error: {e}]*\n\n")
            full_text.append("---\n\n")
    
    # Write output
    output_file.write_text("".join(full_text))
    print(f"\nOCR complete! Output written to: {output_file}")
    print(f"Total pages processed: {len(image_files)}")


if __name__ == "__main__":
    pages_dir = Path(__file__).parent.parent / "docs" / "pages"
    output_file = Path(__file__).parent.parent / "docs" / "PAPER.md"
    
    ocr_pages(pages_dir, output_file)
