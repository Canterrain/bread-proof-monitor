# Bread Proof Monitor

[![Watch the Bread proof Monitor video](https://github.com/user-attachments/assets/b62e2ff1-4c6a-4fda-ae06-24272c835111)](https://www.youtube.com/watch?v=_b9i-Xxo2wM)
**Click the image above to see more about this project on Youtube.**

A dough rise monitor that helps you know when it's time to fold, shape, or bake.

Place the sensor over your dough, choose a bread profile, and follow the rise from your phone. The monitor keeps track of progress, reminds you about folds, and can send a notification when your dough reaches its target.

Built for an ESP32 with a distance sensor and a temperature and humidity sensor. The dashboard runs on the monitor itself and opens in a browser on your home network.

## ✨ Features

- Live dough rise, elapsed time, temperature, and humidity
- Profiles for everyday breads, sourdough, pizza, rolls, and more
- Links to the recipes the profiles are based on
- Fold reminders for recipes with handling steps
- Separate bulk and final proof targets
- Optional phone alerts through ntfy: fold reminders, Ready to Shape, and Ready to Bake
- Pause and resume while you handle the dough
- Feedback that adjusts future targets for your recipe and setup
- Wi-Fi setup from your phone, with no credentials to put in the code
- Installation over USB from your browser

The targets are starting points. Flour, hydration, temperature, and starter or yeast strength all affect a proof. Use the reading alongside how your dough looks and feels.

## 🛠 Hardware

This project is designed around these specific boards and sensor modules:

- [ELEGOO 3PCS ESP-32 Dev Boards](https://amzn.to/46aN2Hq) (affiliate; one board needed)
- [HiLetgo SHT31-D Temperature and Humidity Sensor](https://amzn.to/4xLTMYm) (affiliate)
- [VL53L0X Time-of-Flight Ranging Sensor](https://amzn.to/4xEbyg1) (affiliate)

You'll also need:

- A USB data cable and USB power supply
- A container, and something that holds the distance sensor above the dough

That mount can be as simple as it sounds: the working prototype this project was developed on is just jumper-wired breakouts hot-glued or taped into a lid, on a stand of legs for containers that need one. No PCB is required to build or use this project.

The browser installer is for the original ESP32 chip. ESP32-C3, S3, and P4 boards need different firmware.

### Wiring

Both sensors share the same I²C bus:

| ESP32 | VL53L0X breakout | SHT31 breakout |
| --- | --- | --- |
| 3V3 | VIN / VCC | VIN / VCC |
| GND | GND | GND |
| **GPIO22** | **SDA** | **SDA** |
| **GPIO21** | **SCL** | **SCL** |

Use the pin labels on your particular breakout. This project's SDA and SCL assignments differ from the usual ESP32 defaults.

Mount the distance sensor securely above the dough with room for the tallest expected rise. Keep the sensor as close as that clearance allows. A long distance above a thin layer of dough makes small measurement changes look like much larger changes in rise.

### Optional: Rev A Carrier PCB

If you'd rather not hand-wire and hot-glue sensors into a lid, a custom carrier PCB is available as a drop-in alternative to the wiring table above. Plug-in sockets for the ELEGOO EL-SM-012, ALAMSCN VL53L0X-V2, and HiLetgo GY-SHT31-D modules replace the loose wiring entirely. It's entirely optional; the software doesn't know or care which mounting approach you used. Its [KiCad files and manufacturing package](hardware/kicad/rev-a/README.md) are included, and the [hardware guide](guides/hardware.md) covers ordering and assembly. The Rev A boards were manufactured and assembled by [NextPCB](https://www.nextpcb.com/register?code=Anoraker), who sponsored the PCB portion of this project. See [Sponsored by NextPCB](#-sponsored-by-nextpcb) below.

## 🚀 Quick Start

### Install the software

Use Chrome, Edge, or a Firefox version with Web Serial support on a desktop or laptop. Safari and mobile browsers aren't supported by this installer.

1. Plug the ESP32 into your computer with a USB **data** cable.
2. Open the **[Bread Proof Monitor installer](https://canterrain.github.io/bread-proof-monitor/)**.
3. Click **Connect**, choose your board, and select **Install**.
4. For a first installation, select **Erase device**. Wait for installation to finish and the board to restart.

You don't need Arduino IDE to use the browser installer. If you'd rather build it yourself, see the [build guide](guides/development.md).

### Connect to Wi-Fi

1. Open your phone or computer's **Wi-Fi settings** and join **ProofMonitor-Setup**. The password is **sourdough**.
2. Open [http://192.168.4.1](http://192.168.4.1).
3. Choose your home Wi-Fi network, enter its password, and pick Fahrenheit or Celsius for temperature display.
4. If you want phone alerts, copy the generated ntfy topic and subscribe to it in the ntfy app.
5. Save, wait for the monitor to restart, and reconnect your phone or computer to your home network.
6. Open **[http://proofmonitor.local](http://proofmonitor.local)**.

If that address doesn't open, use the monitor's IP address from your router's connected-device list.

## Using the Monitor

Choose a profile and apply its target. With the empty container and lid in their normal positions, press **Set Empty Setup**. Add the dough, let the lid settle, then press **Set Starting Dough Height**.

The dashboard shows the rise, time, and next step. When a fold is due:

1. Press **Pause Monitoring** before lifting the lid.
2. Fold the dough and put the lid back.
3. Press **Resume Monitoring**, allow the reading to settle, then press **Complete Step**.

When the target is reached, check the dough before pressing **Finish Proof**. For profiles with a monitored final proof, the monitor then prepares the Final stage. Most profiles ask you to calibrate the empty final-proof setup again before setting a new starting height.

Some recipes finish with refrigerated rests or stages that aren't useful to track by height. The monitor tells you when to continue with the linked recipe.

The [usage guide](guides/usage.md) covers calibration, pauses, re-baselining, target confirmation, and starting another batch.

## 🔔 Phone Alerts

Alerts use [ntfy](https://ntfy.sh/). Subscribe to the topic shown during setup to get fold reminders and Ready to Shape or Ready to Bake notifications.

The monitor generates a random topic for you. Treat that topic like a private link: anyone who knows it can subscribe or send messages to it. The dashboard works locally; delivering phone alerts requires internet access.

## Learning Your Dough

After each stage, rate the dough **Underproofed**, **Just Right**, or **Overproofed**, based on what you saw when that stage finished.

Two matching underproofed or overproofed ratings move that recipe and stage's suggested target by five percentage points. Adjustments stay within 25 points of the original suggestion. A Just Right rating clears the streak and keeps the adjustment.

Your feedback is saved on the device. Learned targets are used when you select a profile and when the monitor prepares Final proof. A proof keeps the target you've already applied. **Reset Learned Targets** clears the stored adjustments.

## Updating

Reconnect the ESP32 to your computer and use the same browser installer. For an update from this project's existing 4 MB / Huge APP layout, leave **Erase device** unchecked to retain saved settings. Erasing clears Wi-Fi, calibration, progress, the ntfy topic, and learned targets; repeat first-time setup afterward.

Update between proofs. Wireless updates aren't included.

## Development

The firmware uses Arduino for ESP32. See the [development guide](guides/development.md) for dependencies, local builds, and how to prepare a new browser-installable version.

| Path | What's there |
| --- | --- |
| `bread_proof_monitor.ino` | Startup and main loop |
| `app_state.*`, `proof_state.*` | Saved settings, rise calculations, and proof steps |
| `sensor_reader.*` | Distance, temperature, and humidity readings |
| `recipe_presets.*`, `outcome_feedback.*` | Profiles and learned target adjustments |
| `web_dashboard.*`, `web_assets.h` | Dashboard and setup pages |
| `notifications.*`, `wifi_setup.*` | Phone alerts and Wi-Fi |
| `docs/` | GitHub Pages installer and firmware files |
| `guides/` | Usage, development, and publishing instructions |
| `hardware/` | Carrier PCB design and manufacturing files |
| `tools/` | Local build, upload, and firmware packaging tools |

## 🤝 Sponsored by NextPCB

[<img src="docs/images/nextpcb-logo.png" alt="NextPCB" width="228">](https://www.nextpcb.com/register?code=Anoraker)

The custom carrier boards for this project were manufactured and assembled by **NextPCB**, who sponsored the PCB portion of the Bread Proof Monitor project.

I sent NextPCB the production files from KiCad and ordered five assembled boards. My experience with the process was good, including the review before manufacturing. They spotted an issue in the files I originally submitted and contacted me so I could correct it before the boards went into production. The finished boards arrived cleanly manufactured and assembled, and everything was well packaged when it reached me.

If you want to have your own version of this board manufactured, the KiCad and production files are in [`hardware/kicad/rev-a/`](hardware/kicad/rev-a/README.md), and the [hardware guide](guides/hardware.md) walks through ordering.

### Order Your Custom PCB

- [Order via the NextPCB official website](https://www.nextpcb.com/register?code=Anoraker) (referral link)
- [Free functional circuit testing service](https://www.nextpcb.com/blog/free-functional-testing): NextPCB offers to functionally test your assembled PCBs for free, to verify they work as expected before delivery

## Related Projects

- [Round Weather Display](https://github.com/Canterrain/round-weather-display), a round clock and weather display for Raspberry Pi or ESP32-P4
- [Weather Display](https://github.com/Canterrain/weather-display), a wider clock and weather display for under a cabinet or on a desk
- [Inky Planner](https://github.com/Canterrain/Inky-Planner), a daily planner for Raspberry Pi and Inky e-paper displays

## 📃 License

This project is licensed under the [Creative Commons Attribution–NonCommercial 4.0 International License](https://creativecommons.org/licenses/by-nc/4.0/). See [LICENSE](LICENSE). Third-party libraries retain their own licenses.

© 2026 Josh Hendrickson

Made by [Josh Hendrickson](https://anoraker.com)
