# Rev A Carrier PCB

Reference documentation for the Bread Proof Monitor carrier PCB: a small board that gives the ESP32 module and both sensor breakouts a rigid, repeatable mount on the underside of a lid, using plug-in sockets instead of loose wiring.

## Status

- Schematic and PCB are complete. ERC: 0 errors, 0 warnings. DRC: 0 errors, 1 accepted cosmetic warning (see [Exclusions](#exclusions)).
- Routed at conventional prototype design rules — 0.25 mm trace, 0.6 mm/0.3 mm via, 0.2 mm clearance. No relaxed tolerances.
- All PCB-affecting mechanical decisions are finalized (VL53L0X optical alignment, SHT31 retention approach — see `mechanical-stackup.md`).
- Gerbers, drill files, and the full manufacturing package are generated and ready to order — see [Manufacturing Package](#manufacturing-package).
- Not yet designed: the lid/enclosure itself. Nothing here depends on it; see `mechanical-stackup.md` for the parametric interface (optical bore, vent pocket, standoff heights) it will need to satisfy.

## Bill of Materials

### Carrier-assembled (populated by the PCB assembly house)

| Ref | Part | Qty | Represents |
| --- | --- | ---: | --- |
| J1, J2 | Samtec `SSW-115-01-T-S` (1×15, 2.54 mm, THT) | 2 | Sockets the ELEGOO EL-SM-012 ESP32 module plugs into |
| J3 | Samtec `SSW-106-01-T-S` (1×6, 2.54 mm, THT) | 1 | Socket for the VL53L0X breakout |
| J4 | Samtec `SSW-104-01-T-S` (1×4, 2.54 mm, THT) | 1 | Socket for the SHT31 breakout |

J1 and J2 are the same physical part in two positions (a BOM grouped by MPN reports them as one line, qty 2). All four sockets are the same Samtec SSW family: 2.54 mm pitch, single-row, through-hole, lead-free (`-S`).

`U1` is a silkscreen/mechanical reference only for the ESP32 module's outline — zero pads, `in_bom=no`, `dnp=yes`. It never appears in a BOM export.

### User-installed (not part of the PCB order)

| Plugs into | Part | Amazon ASIN |
| --- | --- | --- |
| J1 + J2 | ELEGOO EL-SM-012 (ESP32 dev module) | B0D8T53CQ5 |
| J3 | ALAMSCN black VL53L0X-V2 breakout | B0DGL2P6XC |
| J4 | HiLetgo GY-SHT31-D breakout | B07ZSZW92J |

The EL-SM-012 plugs directly into J1/J2 using its own existing pin rows — nothing to solder. The VL53L0X-V2 and SHT31 breakouts (J3, J4) ship with loose 2.54 mm male header strips that need to be soldered onto the breakout before it plugs into the carrier.

## Pinout

### EL-SM-012 (ESP32 module) pin map

Verified directly against the [EL-SM-012 manual's pinout diagram](https://manuals.plus/asin/B0D8T53CQ5) (Section 5, Figure 5.1). Only the four pads below carry nets; the rest of the header exists for mechanical support only.

| Pad | Function | Pad | Function |
| ---: | --- | ---: | --- |
| 1 | VIN (5V) | 16 | **3V3** |
| 2 | **GND** | 17 | **GND** |
| 3 | GPIO13 | 18 | GPIO15 |
| 4 | GPIO12 | 19 | GPIO2 |
| 5 | GPIO14 | 20 | GPIO4 |
| 6 | GPIO27 | 21 | GPIO16 |
| 7 | GPIO26 | 22 | GPIO17 |
| 8 | GPIO25 | 23 | GPIO5 |
| 9 | GPIO33 | 24 | GPIO18 |
| 10 | GPIO32 | 25 | GPIO19 |
| 11 | GPIO35 | 26 | **GPIO21 (SCL)** |
| 12 | GPIO34 | 27 | GPIO3 (RX0) |
| 13 | GPIO39 (VN) | 28 | GPIO1 (TX0) |
| 14 | GPIO36 (VP) | 29 | **GPIO22 (SDA)** |
| 15 | EN | 30 | GPIO23 |

**Warning:** pad 1 is VIN — raw 5V from USB — not 3V3. Only pads 16 (3V3) and 17 (GND) feed the sensor rail; both sensors are 3.3V-only parts. All four connected pads (3V3, GND, SDA, SCL) sit on the same header column (16-30); the other column (1-15) carries no nets on this board.

The GPIO22 → SDA / GPIO21 → SCL assignment is a firmware convention, not a hardware requirement — it matches the wiring already used by the ESP32 sketch itself. The EL-SM-012's own silkscreen shows a different default for the Wire library; this board's net assignment intentionally overrides that default.

### Connector and mounting-hole reference

`LID_ORIGIN` (0, 0) is the geometric center of the dough/container reference plane, and doubles as `TOF_ORIGIN` — the VL53L0X's optical axis is placed exactly there by construction (J3 is rotated 180° so its optical center, not its mounting-hole midpoint, lands on the datum). Coordinates below are relative to `LID_ORIGIN`; +X toward the antenna notch, +Y toward the ESP32 header.

| Feature | Coordinate (mm) | Notes |
| --- | ---: | --- |
| J3 (VL53L0X) mounting holes | (-10, -2.2) / (+10, -2.2) | Hole-midpoint sits at (0, -2.2), not `LID_ORIGIN` — the sensor's actual optical axis is what's aligned to the datum instead |
| J4 (SHT31) footprint origin (pin 1 / VIN) | (0, -29) | Not a mechanical datum. Retention is a lid/enclosure pocket-clip, not a PCB feature |
| U1 (ESP32 module outline) footprint origin | (0, +27) | Mechanical reference only, not a BOM item |
| J1 / J2 (ESP32 carrier sockets) | (-17.78, +14.18) / (-17.78, +39.82) | Local pin 1 of each |
| MH1 / MH2 / MH3 (carrier mounting holes) | (+40, 0) / (-40, 0) / (0, -45) | 3.2 mm NPTH, M3 clearance, provisional pending host hardware |

## Board Outline

A circle with a USB-C access notch and an antenna overhang notch, rather than a plain circle or a custom polygon — a plain circle large enough to clear the ESP32 module would need to be bigger still and wouldn't give cable/antenna clearance on its own.

- **Base circle:** radius 52 mm, centered on `LID_ORIGIN`. Overall envelope: X/Y from -52.0 to +52.0 mm (~104 × 104 mm).
- **USB-C notch:** board material stops at X = -25 mm, open from there to the edge across Y = 17-37 mm. The EL-SM-012's USB-C connector sits just past this cut, fully in open air.
- **Antenna notch:** board material stops at X = +21 mm, open across the same Y range, 3.22 mm past J1/J2's last pin — giving the WROOM antenna genuine physical overhang rather than relying only on a copper keepout.
- The board's lower size bound is the EL-SM-012 module's own footprint (51.74 × 29.00 mm); its farthest structurally-needed corner sits 50.3 mm from `LID_ORIGIN`, leaving a 1.7 mm margin at the 52 mm radius.

## Antenna Clearance

- `ESP32_ANTENNA_KEEP_OUT`, on both F.Cu and B.Cu: X 9-65 mm, Y 15-39 mm (56 × 24 mm) — no tracks, vias, or zone fills permitted inside it.
- Board material itself ends at X = 21 mm (the antenna notch above), so this region has no FR4 at all, not just no copper.
- The SDA trace's closest approach (pad 29, local X = +15.24 mm) jogs into its routing lane above the header row before heading down the board, so it never enters the keepout's Y band.

## Routing

- **Shared trunks, not per-destination lanes.** 3V3, SDA, and SCL each get one trunk from J2, which tees off to J3 and continues to J4 — not a separate trunk per destination. This keeps routing to 3 lanes at conventional design rules.
- **GND has no explicit trace.** J2, J3, and J4's GND pads are through-hole and sit directly inside the filled F.Cu/B.Cu ground pours; three stitching vias tie the two copper layers together away from the signal corridor. Verified directly against KiCad's own connectivity graph (not inferred from geometry) — J2's GND pad and both sensors' GND pads resolve to the same connected copper network.
- All routing lives on B.Cu, with a handful of via hops to F.Cu to resolve a few crossings.
- 15 vias total: 12 signal-routing detour vias (3V3 ×8, SCL ×4, SDA ×0) plus 3 GND stitching vias. Minimum via-to-via spacing: 0.7 mm edge-to-edge.

## Mechanical Clearances

No copper or courtyard overlap anywhere on the board:

- J1 (top, nearest neighbor to J3) to J3 (bottom): 3.93 mm Y gap.
- J3 to J4 (both bottom): 4.60 mm Y gap.

## Exclusions

1. **Silkscreen clipped by the board edge** — U1's own reference outline rectangle on F.SilkS is clipped at the USB-C notch, since the real EL-SM-012 module genuinely overhangs that cutout by design. Cosmetic only; has no fabrication impact.

## Manufacturing Package

`RevA_Manufacturing_2026-08-26/` contains the complete fabrication and assembly output — Gerbers, Excellon drill files with maps and a report, the final assembly BOM, a placement/position file, a fabrication drawing, and `RevA_Gerbers_NextPCB.zip` (Gerbers + drill files pre-zipped for direct upload to NextPCB's quote tool). See that directory's own `README.md` for the full contents list, fabrication notes, and its change history.

## References

- [EL-SM-012 manual and dimensional drawing](https://manuals.plus/asin/B0D8T53CQ5) — pinout diagram in Section 5, Figure 5.1
- [HiLetgo GY-SHT31-D product listing, ASIN B07ZSZW92J](https://www.amazon.com/dp/B07ZSZW92J)
- [ALAMSCN VL53L0X-V2 product listing, ASIN B0DGL2P6XC](https://www.amazon.com/dp/B0DGL2P6XC)
