# Ordering and Building the Carrier PCB

[Back to the project](../README.md)

This covers getting a Rev A carrier PCB made and turning it into a working sensor mount. For the board's full technical reference (pinout, dimensions, design rules, verification results), see [`hardware/kicad/rev-a/README.md`](../hardware/kicad/rev-a/README.md).

The lid and enclosure this carrier mounts into aren't designed yet, so this guide stops at a working, bench-wired sensor mount rather than a finished physical unit.

## 1. Order the PCB

The fabrication files are in [`hardware/kicad/rev-a/RevA_Manufacturing_2026-08-26/`](../hardware/kicad/rev-a/RevA_Manufacturing_2026-08-26/). It's a conventional 2-layer board at standard prototype tolerances — no special fabrication options are needed.

- **Bare board only:** upload `RevA_Gerbers_NextPCB.zip` (or the `gerbers/` and `drill/` folders directly) to your fab house's quote tool. You'll receive an unpopulated board and need to hand-solder the four sockets yourself — see the carrier-assembled parts table in [`hardware/kicad/rev-a/README.md`](../hardware/kicad/rev-a/README.md#bill-of-materials) (or `bom/RevA_assembly_BOM.csv` directly) for the exact Samtec part numbers.
- **Board with sockets pre-assembled:** if your fab offers PCB assembly (NextPCB does), upload the Gerbers/drill files for fabrication and also supply `bom/RevA_assembly_BOM.csv` and `position/RevA_placement.csv` for the assembly step. All four sockets are through-hole, so hand or wave soldering is the expected assembly method — a pick-and-place file has limited value here, but it's included for completeness.

Either way, you're only ordering and paying for the bare carrier board and its four sockets — the ESP32 module and both sensor breakouts are never part of this order.

## 2. Buy the plug-in modules

These are ordinary off-the-shelf parts, bought separately from wherever you like:

| Plugs into | Part | Amazon ASIN |
| --- | --- | --- |
| J1 + J2 | ELEGOO EL-SM-012 (ESP32 dev module) | B0D8T53CQ5 |
| J3 | ALAMSCN black VL53L0X-V2 breakout | B0DGL2P6XC |
| J4 | HiLetgo GY-SHT31-D breakout | B07ZSZW92J |

## 3. Prepare the sensor breakouts

The VL53L0X and SHT31 breakouts ship with loose 2.54 mm male header strips, not pre-soldered pins. Solder a header onto each one before it can plug into the carrier.

The EL-SM-012 needs no preparation — it already has its own male pin rows fixed to the module and plugs straight into J1/J2.

## 4. Assemble

1. Plug the EL-SM-012 into the J1/J2 sockets (top side of the carrier).
2. Plug the VL53L0X breakout into J3 (underside).
3. Plug the SHT31 breakout into J4 (underside), sensing element facing outward.

**Before powering on, double-check orientation on all three.** These are plain single-row headers into single-row sockets — nothing mechanically prevents a shifted or reversed insertion. Confirm each module's pin 1 lines up with its socket's pin 1 against the board's silkscreen. Getting this wrong on the ESP32 module in particular can put 5V (VIN) where the sensors expect 3.3V.

## 5. Mount to a lid

Carrier mounting holes MH1-MH3 (3.2 mm NPTH, M3 clearance) are in place for this, but the lid/enclosure design that uses them isn't finished. Until it is, this is a DIY step — see [`mechanical-stackup.md`](../hardware/kicad/rev-a/mechanical-stackup.md) for the parametric interface (optical bore, vent pocket, standoff heights) any lid design needs to satisfy, and the wiring table in the main [README](../README.md#wiring) for a non-PCB mounting approach in the meantime.

## 6. First power-up

Once assembled, treat it like any other Bread Proof Monitor board: follow the main README's [Quick Start](../README.md#-quick-start) to install the firmware over USB and connect it to Wi-Fi.
