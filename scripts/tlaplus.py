#!/usr/bin/env python3
"""
Unified TLC helper: replaces bootstrap.{sh,ps1} and run-tlc.{sh,ps1}.

Subcommands:
  - bootstrap : download tla2tools.jar into tools/ (or custom dir)
  - run       : ensure jar exists, then run TLC and tee output to log
"""

from __future__ import annotations

import argparse
import shutil
import subprocess
import sys
from pathlib import Path
from typing import Iterable, List
from urllib.request import urlopen


DEFAULT_VERSION = "1.8.0"
DEFAULT_TOOLS_DIR = "tools"
DEFAULT_RESULTS_DIR = "ci-results"
DEFAULT_MODULE = "STLRepairAlgo"
DEFAULT_CONFIG = "models/STLRepairAlgo.cfg"
DEFAULT_SPEC_DIR = "spec"
DEFAULT_JAVA = "java"


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="TLA+ TLC helper (bootstrap + run) in one script."
    )

    parser.add_argument(
        "--tools-dir",
        default=DEFAULT_TOOLS_DIR,
        help=f"Directory for tla2tools.jar (default: {DEFAULT_TOOLS_DIR!r}).",
    )

    parser.add_argument(
        "--version",
        default=DEFAULT_VERSION,
        help=f"TLA+ tools version tag (default: {DEFAULT_VERSION!r}).",
    )

    parser.add_argument(
        "--url-template",
        default=(
            "https://github.com/tlaplus/tlaplus/releases/"
            "download/v{version}/tla2tools.jar"
        ),
        help="Template URL for tla2tools.jar; {version} will be substituted.",
    )

    subparsers = parser.add_subparsers(
        dest="command",
        required=True,
        help="Subcommands.",
    )

    bootstrap = subparsers.add_parser(
        "bootstrap",
        help="Download tla2tools.jar into tools-dir if missing.",
    )

    bootstrap.add_argument(
        "--force",
        action="store_true",
        help="Re-download even if tla2tools.jar already exists.",
    )

    run_p = subparsers.add_parser(
        "run",
        help="Run TLC on a TLA+ module, teeing output to a log file.",
    )

    run_p.add_argument(
        "module",
        nargs="?",
        default=DEFAULT_MODULE,
        help=f"TLA+ module name without .tla (default: {DEFAULT_MODULE!r}).",
    )

    run_p.add_argument(
        "-c",
        "--config",
        default=DEFAULT_CONFIG,
        help=f"Path to TLC config file (default: {DEFAULT_CONFIG!r}).",
    )

    run_p.add_argument(
        "--spec-dir",
        default=DEFAULT_SPEC_DIR,
        help=f"Directory containing .tla specs (default: {DEFAULT_SPEC_DIR!r}).",
    )

    run_p.add_argument(
        "--results-dir",
        default=DEFAULT_RESULTS_DIR,
        help=f"Directory for TLC log files (default: {DEFAULT_RESULTS_DIR!r}).",
    )

    run_p.add_argument(
        "--java",
        default=DEFAULT_JAVA,
        help=f"Java executable (default: {DEFAULT_JAVA!r}).",
    )

    run_p.add_argument(
        "--extra-java-arg",
        action="append",
        default=[],
        metavar="ARG",
        help="Extra argument to pass to Java (can be repeated).",
    )

    run_p.add_argument(
        "--no-bootstrap",
        action="store_true",
        help="Skip automatic bootstrap before running TLC.",
    )

    return parser.parse_args()


def ensure_dir(path: Path) -> None:
    path.mkdir(parents=True, exist_ok=True)


def build_jar_url(version: str, url_template: str) -> str:
    return url_template.format(version=version)


def download_tla_tools(url: str, target: Path, overwrite: bool = False) -> None:
    if target.exists() and not overwrite:
        print(f"tla2tools.jar already present at {target}.")
        return

    ensure_dir(target.parent)
    tmp = target.with_suffix(target.suffix + ".tmp")

    print(f"Downloading tla2tools.jar from {url} ...")
    try:
        with urlopen(url) as resp, tmp.open("wb") as f:
            chunk = resp.read(8192)
            while chunk:
                f.write(chunk)
                chunk = resp.read(8192)
    except Exception as exc:
        if tmp.exists():
            tmp.unlink()
        raise SystemExit(f"Failed to download tla2tools.jar: {exc}") from exc

    tmp.replace(target)
    target.chmod(0o644)
    print(f"Saved tla2tools.jar to {target}.")


def ensure_java_available(java_exe: str) -> None:
    if shutil.which(java_exe) is None:
        raise SystemExit(
            f"Java executable {java_exe!r} not found in PATH. "
            "Install Java or pass --java with a full path."
        )


def tee_process_output(
    proc: subprocess.Popen,
    log_path: Path,
) -> int:
    ensure_dir(log_path.parent)
    with log_path.open("w", encoding="utf-8", errors="replace") as log:
        assert proc.stdout is not None
        for line in proc.stdout:
            sys.stdout.write(line)
            sys.stdout.flush()
            log.write(line)
            log.flush()
    return proc.wait()


def build_tlc_command(
    java_exe: str,
    extra_java_args: Iterable[str],
    jar_path: Path,
    cfg_path: Path,
    module_path: Path,
) -> List[str]:
    cmd: List[str] = [java_exe]
    cmd.extend(extra_java_args)
    cmd.extend(
        [
            "-cp",
            str(jar_path),
            "tlc2.TLC",
            "-config",
            str(cfg_path),
            str(module_path),
        ]
    )
    return cmd


def run_tlc(
    java_exe: str,
    extra_java_args: Iterable[str],
    tools_dir: Path,
    spec_dir: Path,
    module: str,
    cfg: Path,
    results_dir: Path,
) -> int:
    ensure_java_available(java_exe)

    jar_path = tools_dir / "tla2tools.jar"
    if not jar_path.exists():
        raise SystemExit(
            f"{jar_path} does not exist. Run with 'bootstrap' first "
            "or omit --no-bootstrap on the 'run' command."
        )

    module_path = spec_dir / f"{module}.tla"
    if not module_path.exists():
        raise SystemExit(f"Spec file not found: {module_path}")

    cfg_path = cfg
    if not cfg_path.exists():
        raise SystemExit(f"Config file not found: {cfg_path}")

    ensure_dir(results_dir)
    log_path = results_dir / f"{module}.tlc.log"

    cmd = build_tlc_command(
        java_exe=java_exe,
        extra_java_args=list(extra_java_args),
        jar_path=jar_path,
        cfg_path=cfg_path,
        module_path=module_path,
    )

    print("Running TLC with command:")
    print("  " + " ".join(str(c) for c in cmd))
    print(f"Logging output to {log_path}")

    proc = subprocess.Popen(
        cmd,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
        bufsize=1,
        universal_newlines=True,
    )
    return tee_process_output(proc, log_path)


def main() -> None:
    args = parse_args()
    tools_dir = Path(args.tools_dir)
    url = build_jar_url(args.version, args.url_template)
    jar_target = tools_dir / "tla2tools.jar"

    if args.command == "bootstrap":
        download_tla_tools(url, jar_target, overwrite=args.force)
        return

    if args.command == "run":
        if not args.no_bootstrap:
            download_tla_tools(url, jar_target, overwrite=False)

        exit_code = run_tlc(
            java_exe=args.java,
            extra_java_args=args.extra_java_arg,
            tools_dir=tools_dir,
            spec_dir=Path(args.spec_dir),
            module=args.module,
            cfg=Path(args.config),
            results_dir=Path(args.results_dir),
        )
        raise SystemExit(exit_code)

    raise SystemExit("No command given; use --help for usage.")


if __name__ == "__main__":
    main()
