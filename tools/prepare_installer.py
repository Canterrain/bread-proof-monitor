#!/usr/bin/env python3
"""Compile and package flash-ready ESP32 images for the static USB installer."""

import argparse
import hashlib
import json
from pathlib import Path
import re
import shlex
import shutil
import struct
import subprocess
import tempfile
from datetime import datetime, timezone

ROOT = Path(__file__).resolve().parents[1]
FQBN = "esp32:esp32:esp32:PartitionScheme=huge_app"
SKETCH = "bread_proof_monitor.ino"
PARTS = [
    (0x1000, SKETCH + ".bootloader.bin", "bootloader.bin", 0x7000),
    (0x8000, SKETCH + ".partitions.bin", "partitions.bin", 0x1000),
    (0xE000, "boot_app0.bin", "boot_app0.bin", 0x2000),
    (0x10000, SKETCH + ".bin", "firmware.bin", 0x300000),
]


def validate_images(images):
    """Reject another chip/layout or any write reaching saved-setting sectors."""
    for offset, _, name, maximum in PARTS:
        data = images[name]
        if not data or len(data) > maximum:
            raise ValueError(f"Unexpected size for {name}: {len(data)} bytes")
        # esptool erases whole 4 KB sectors for each part, even without a full erase.
        end = (offset + len(data) + 0xFFF) & ~0xFFF
        if offset < 0xE000 and end > 0x9000:
            raise ValueError(f"{name} would overwrite saved settings")

    for name in ("bootloader.bin", "firmware.bin"):
        data = images[name]
        if len(data) < 24 or data[0] != 0xE9 or int.from_bytes(data[12:14], "little") != 0:
            raise ValueError(f"{name} is not an original ESP32 image")
        # Arduino's elf2image/bootloader generation already patches these fields.
        # Web Serial must not depend on the upload CLI to patch them later.
        if data[2:4] != bytes([2, 0x2F]):
            raise ValueError(f"{name} must already have DIO / 80 MHz / 4 MB flash headers")

    table = images["partitions.bin"]
    found = {}
    md5_checked = False
    for start in range(0, len(table) - 31, 32):
        magic, kind, subtype, offset, size, raw_name, flags = struct.unpack_from("<HBBII16sI", table, start)
        if magic == 0xEBEB:
            if hashlib.md5(table[:start]).digest() != table[start + 16:start + 32]:
                raise ValueError("Partition-table checksum does not match")
            md5_checked = True
            break
        if magic != 0x50AA:
            raise ValueError("Invalid partition table")
        name = raw_name.rstrip(b"\0").decode("ascii")
        if name in found:
            raise ValueError("Duplicate partition name")
        found[name] = (kind, subtype, offset, size, flags)
    expected = {
        "nvs": (1, 2, 0x9000, 0x5000, 0),
        "otadata": (1, 0, 0xE000, 0x2000, 0),
        "app0": (0, 0x10, 0x10000, 0x300000, 0),
        "spiffs": (1, 0x82, 0x310000, 0xE0000, 0),
        "coredump": (1, 3, 0x3F0000, 0x10000, 0),
    }
    if not md5_checked or found != expected:
        raise ValueError("Partition layout differs from the supported 4 MB Huge APP layout")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--version", required=True, help="Release label, e.g. 0.1.0-preview")
    args = parser.parse_args()
    if not re.fullmatch(r"[A-Za-z0-9][A-Za-z0-9.+-]{0,63}", args.version):
        parser.error("Use a version containing letters, digits, dots, plus signs, or hyphens")

    cli = shutil.which("arduino-cli")
    if not cli:
        parser.error("Arduino CLI is required; see guides/development.md")

    scratch = ROOT / ".arduino"
    scratch.mkdir(exist_ok=True)
    build = scratch / "installer-build"
    source_files = sorted(p for p in ROOT.iterdir() if p.suffix in (".ino", ".cpp", ".h"))
    fingerprint = hashlib.sha256()
    with tempfile.TemporaryDirectory(prefix="installer-source-", dir=scratch) as temporary:
        stage = Path(temporary) / "bread_proof_monitor"
        stage.mkdir()
        for source in source_files:
            data = source.read_bytes()
            fingerprint.update(source.name.encode() + b"\0" + data + b"\0")
            (stage / source.name).write_bytes(data)
        subprocess.run([cli, "compile", "--fqbn", FQBN, "--build-path", str(build), str(stage)], check=True)

    # Validate the freshly generated upload plan, not arbitrary pre-existing binaries.
    lines = (build / "flash_args").read_text().splitlines()
    if shlex.split(lines[0]) != ["--flash-mode", "dio", "--flash-freq", "80m", "--flash-size", "4MB"]:
        raise ValueError("Unexpected flash configuration")
    plan = [(int(offset, 0), name) for offset, name in (shlex.split(line) for line in lines[1:] if line.strip())]
    if plan != [(offset, source) for offset, source, _, _ in PARTS]:
        raise ValueError("Unexpected upload offsets or files")
    images = {name: (build / source).read_bytes() for _, source, name, _ in PARTS}
    validate_images(images)

    manifest = {
        "name": "Bread Proof Monitor",
        "version": args.version,
        "new_install_prompt_erase": True,
        "new_install_improv_wait_time": 0,
        "builds": [{"chipFamily": "ESP32", "parts": [
            {"path": name, "offset": offset} for offset, _, name, _ in PARTS
        ]}],
    }
    info = {
        "version": args.version,
        "builtAt": datetime.now(timezone.utc).isoformat(timespec="seconds"),
        "fqbn": FQBN,
        "sourceSha256": fingerprint.hexdigest(),
        "sourceFiles": [p.name for p in source_files],
        "parts": [{"path": name, "offset": offset, "bytes": len(images[name]),
                   "sha256": hashlib.sha256(images[name]).hexdigest()} for offset, _, name, _ in PARTS],
    }
    output = ROOT / "docs" / "firmware"
    output.mkdir(parents=True, exist_ok=True)
    # Publish the manifest last; no output files are touched before validation succeeds.
    for name, data in images.items():
        (output / name).write_bytes(data)
    (output / "build-info.json").write_text(json.dumps(info, indent=2) + "\n")
    checksums = "".join(f"{part['sha256']}  {part['path']}\n" for part in info["parts"])
    (output / "SHA256SUMS").write_text(checksums)
    (output / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n")
    print(f"Prepared {args.version}: four validated ESP32 images in docs/firmware/.")
    print("No board was flashed and nothing was published.")


if __name__ == "__main__":
    main()
