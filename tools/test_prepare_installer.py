"""Validate the distributed firmware and reject unsafe packaging inputs."""

import hashlib
import json
from pathlib import Path
import struct
import unittest

from prepare_installer import ROOT, PARTS, validate_images


class InstallerPackageTests(unittest.TestCase):
    def setUp(self):
        folder = ROOT / "docs" / "firmware"
        self.images = {name: (folder / name).read_bytes() for _, _, name, _ in PARTS}

    def test_release_files_match_manifest_checksums_and_layout(self):
        validate_images(self.images)
        folder = ROOT / "docs" / "firmware"
        manifest = json.loads((folder / "manifest.json").read_text())
        info = json.loads((folder / "build-info.json").read_text())
        self.assertEqual(manifest["version"], info["version"])
        self.assertTrue(manifest["new_install_prompt_erase"])
        self.assertEqual(manifest["new_install_improv_wait_time"], 0)
        self.assertEqual(manifest["builds"], [{"chipFamily": "ESP32", "parts": [
            {"path": name, "offset": offset} for offset, _, name, _ in PARTS
        ]}])
        for part in info["parts"]:
            data = self.images[part["path"]]
            self.assertEqual(len(data), part["bytes"])
            self.assertEqual(hashlib.sha256(data).hexdigest(), part["sha256"])
        for line in (folder / "SHA256SUMS").read_text().splitlines():
            digest, name = line.split()
            self.assertEqual(digest, hashlib.sha256(self.images[name]).hexdigest())

    def test_firmware_matches_current_source(self):
        info = json.loads((ROOT / "docs/firmware/build-info.json").read_text())
        fingerprint = hashlib.sha256()
        files = sorted(p for p in ROOT.iterdir() if p.suffix in (".ino", ".cpp", ".h"))
        self.assertEqual([p.name for p in files], info["sourceFiles"])
        for source in files:
            fingerprint.update(source.name.encode() + b"\0" + source.read_bytes() + b"\0")
        self.assertEqual(fingerprint.hexdigest(), info["sourceSha256"], "Rebuild the installer after firmware edits")

    def test_no_flash_part_erases_the_settings_partition(self):
        # Use sector-rounded write ranges, since a partial write erases the whole sector.
        for offset, _, name, _ in PARTS:
            first_sector = offset // 4096
            last_sector = (offset + len(self.images[name]) - 1) // 4096
            self.assertTrue(set(range(first_sector, last_sector + 1)).isdisjoint(range(9, 14)), name)

    def test_rejects_a_different_chip(self):
        image = bytearray(self.images["firmware.bin"])
        image[12:14] = b"\x02\x00"
        self.images["firmware.bin"] = image
        with self.assertRaisesRegex(ValueError, "not an original ESP32"):
            validate_images(self.images)

    def test_rejects_flash_headers_that_need_upload_time_patching(self):
        image = bytearray(self.images["bootloader.bin"])
        image[2] = 0  # QIO instead of the expected DIO image.
        self.images["bootloader.bin"] = image
        with self.assertRaisesRegex(ValueError, "flash headers"):
            validate_images(self.images)

    def test_rejects_oversized_bootloader_and_application(self):
        for _, _, name, maximum in (PARTS[0], PARTS[3]):
            with self.subTest(part=name):
                oversized = dict(self.images)
                oversized[name] = oversized[name] + b"\xff" * (maximum + 1 - len(oversized[name]))
                with self.assertRaisesRegex(ValueError, "Unexpected size"):
                    validate_images(oversized)

    def test_rejects_a_different_layout_even_with_a_valid_checksum(self):
        table = bytearray(self.images["partitions.bin"])
        struct.pack_into("<I", table, 8, 0x4000)  # Shrink NVS from 0x5000 to 0x4000.
        for start in range(0, len(table), 32):
            if table[start:start + 2] == b"\xeb\xeb":
                table[start + 16:start + 32] = hashlib.md5(table[:start]).digest()
                break
        self.images["partitions.bin"] = table
        with self.assertRaisesRegex(ValueError, "Partition layout differs"):
            validate_images(self.images)

    def test_rejects_a_corrupt_partition_table(self):
        table = bytearray(self.images["partitions.bin"])
        table[8] ^= 1
        self.images["partitions.bin"] = table
        with self.assertRaisesRegex(ValueError, "checksum"):
            validate_images(self.images)


if __name__ == "__main__":
    unittest.main()
