# Building the Firmware

[Back to the project](../README.md)

Most users can use the browser installer. These instructions are for changing the firmware or packaging a new version.

## Arduino IDE

The sketch folder must be named `bread_proof_monitor`, matching `bread_proof_monitor.ino`. If you download a GitHub ZIP, rename the extracted folder before opening it.

1. Install the ESP32 board package by Espressif Systems.
2. Choose **ESP32 Dev Module**.
3. Choose **Huge APP (3MB No OTA/1MB SPIFFS)** under Partition Scheme. This is the layout used for the distributed firmware.
4. Install **Adafruit VL53L0X** and **Adafruit SHT31 Library**, including their dependencies.
5. Open `bread_proof_monitor.ino`, compile, then upload.

The current local build uses:

| Component | Version |
| --- | --- |
| Arduino ESP32 core | 3.3.10 |
| Adafruit VL53L0X | 1.2.5 |
| Adafruit SHT31 Library | 2.2.2 |
| Adafruit BusIO | 1.17.4 |

The firmware also uses WiFi, WebServer, ESPmDNS, Preferences, HTTPClient, and WiFiClientSecure from the ESP32 core. Keep **GPIO22 = SDA** and **GPIO21 = SCL**.

## Arduino CLI

From a checkout named `bread_proof_monitor`, with Arduino CLI and those dependencies installed:

```bash
arduino-cli compile --fqbn esp32:esp32:esp32:PartitionScheme=huge_app --build-path .arduino/build .
arduino-cli upload --fqbn esp32:esp32:esp32:PartitionScheme=huge_app --port YOUR_SERIAL_PORT --input-dir .arduino/build .
```

Replace `YOUR_SERIAL_PORT` with the port reported by `arduino-cli board list`.

On macOS or Linux with zsh, the existing convenience scripts are also available:

```bash
./tools/compile.sh
./tools/upload.sh
./tools/monitor.sh
```

Their defaults are the same Huge APP board configuration, `/dev/cu.usbserial-0001`, and 115200 baud. Override the port with `PORT=/dev/your-port ./tools/upload.sh` or the monitor speed with `MONITOR_BAUD=74880 ./tools/monitor.sh`.

## Prepare a Browser Installer Build

From the project root:

```bash
python3 tools/prepare_installer.py --version 0.1.0-preview
```

Use an explicit version each time you prepare a release. The script compiles the current source, validates the chip, image headers, partition layout, and flash regions, then writes the four firmware parts, manifest, build information, and checksums into `docs/firmware/`.

It requires Python 3 and the Arduino CLI environment described above. It does not upload to a board or publish anything. It stages the sketch under `.arduino/`, so packaging also works if the repository folder has a different name.

Check the distributed package and its protection against wrong-chip images, changed layouts, and writes over saved settings:

```bash
python3 -m unittest discover -s tools -p 'test_*.py'
```

The installer deliberately uses separate flash parts. Sending the padded 4 MB merged image would also overwrite the space containing device settings even when the user did not request an erase. The packaged parts avoid that settings region. Keep the current partition layout when offering an update that preserves settings.

Preview the installer with:

```bash
python3 -m http.server 8765 --bind 127.0.0.1 --directory docs
```

Open [http://localhost:8765](http://localhost:8765). Web Serial requires HTTPS when hosted; localhost is suitable for local development. The installer library is loaded from ESP Web Tools and needs internet access.

## Diagnostics

The dashboard polls `/data` every 15 seconds. Along with visible fields, that endpoint includes segment baseline, completed-progress offset, and retained peak values for inspecting the rise calculations. Use the device's LAN IP for repeated scripted reads if `.local` resolution is unreliable.

The serial monitor runs at 115200 baud and reports sensors, Wi-Fi connection details, proof state, and notification results. Keep device logs and flash dumps out of public releases: they may contain your Wi-Fi name or ntfy topic.

## Publishing

See [Publishing on GitHub](publishing.md) for the repository and GitHub Pages setup, release checks, and preparing future updates.
