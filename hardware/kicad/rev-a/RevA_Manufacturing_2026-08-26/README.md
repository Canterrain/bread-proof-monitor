# Bread Proof Monitor — Rev A Manufacturing Package

Generated 2026-08-26, revised 2026-08-27 (DFA fix + silkscreen-width
correction, see below) from
`rev-a-carrier.kicad_pcb` / `rev-a-carrier.kicad_sch`.
ERC: 0 errors, 0 warnings. DRC: 0 errors, 1 accepted cosmetic warning (see
`../README.md` "Exclusions"). Board: Ø104 mm, 2-layer, conventional
prototype design rules (0.25 mm trace / 0.20 mm clearance / 0.6 mm via,
0.3 mm drill). No relaxed tolerances.

## 2026-08-27 DFA fix — J3/J4 manufacturing silkscreen corrected

NextPCB's DFA review (report T-E110466AA1) flagged: *"The component is
vertical pin header while the silk in Gerber is for right-angle
component, is the part number correct?"* (J3, J4). The MPNs were always
correct (`SSW-106-01-T-S`, `SSW-104-01-T-S` — both straight/vertical THT
sockets); the problem was the **manufacturing silkscreen graphic**, not
the part number. J3/J4's silkscreen had been showing the outline of the
much larger *removable sensor breakout module* (the VL53L0X/SHT31 board
envelope — useful for enclosure/mechanical design, not for assembly) on
the same silkscreen layer as the real socket, with the actual 6-/4-pin
socket body floating inside it. NextPCB's assembly reviewer reasonably
read that large, oddly-placed outline as "the connector body" and it
didn't match a straight-pin part.

**Fix:** the module-envelope outlines moved to `Dwgs.User` (a
documentation/mechanical layer, not exported to manufacturing Gerbers),
each labeled `VL53L0X MODULE ENVELOPE` / `SHT31 MODULE ENVELOPE`. New,
correctly-sized silkscreen outlines were added on `B.Silkscreen` sized to
the actual Samtec SSW body-length formula (`2.54mm × N + 0.51mm`) for
length and Samtec's published single-row body width (2.41 mm / .095") for
width — 15.75 mm × 2.41 mm for J3 (`SSW-106-01-T-S`), 10.67 mm × 2.41 mm
for J4 (`SSW-104-01-T-S`), centered on the real pin rows. (An initial pass
used an unverified 5.0 mm placeholder width, corrected 2026-08-27 to the
2.41 mm figure per Samtec's TH SSW drawing.) J4's silkscreen
reference-designator text was nudged 1.2 mm to stay clear of the new,
tighter outline (was overlapping it). No copper, mask, drill, board
outline, placement, or BOM content changed — verified by content-diff
against the prior approved package: only `B_Silkscreen.gbr` changed.

## What NextPCB assembles

Only the four carrier-side sockets below are populated by the assembly
house. **Nothing else is on this BOM** — the ESP32 module and both sensor
breakouts are user-installed after assembly (see next section).

| Designator | Qty | Manufacturer | MPN | Package |
| --- | ---: | --- | --- | --- |
| J1, J2 | 2 | Samtec | `SSW-115-01-T-S` | 1×15, 2.54 mm pitch, THT |
| J3 | 1 | Samtec | `SSW-106-01-T-S` | 1×6, 2.54 mm pitch, THT |
| J4 | 1 | Samtec | `SSW-104-01-T-S` | 1×4, 2.54 mm pitch, THT |

All four are the same Samtec SSW family: 2.54 mm pitch, single-row,
through-hole, lead-free solderable (`-S` row option). J1/J2 are
electrically and mechanically identical parts (same footprint, same MPN)
— they're the two socket rows the ELEGOO EL-SM-012 ESP32 module plugs
into; a BOM grouped by MPN correctly reports them as one line, qty 2, so
they can't be mistaken for a single socket.

See `bom/RevA_assembly_BOM.csv` for the machine-readable version and
`position/RevA_placement.csv` for X/Y/rotation (all four parts are
through-hole, so a pick-and-place file has limited value for this board —
included for completeness, but hand/wave-solder placement referencing the
silkscreen reference designators is the expected assembly method).

## What is NOT on this BOM — user-installed plug-in modules

These three modules are **not procured or soldered by NextPCB**. They are
purchased separately and plugged into the sockets above by the end user
after the carrier PCB is assembled:

| Plugs into | Exact part | Amazon ASIN |
| --- | --- | --- |
| J1 + J2 | ELEGOO EL-SM-012 (ESP32 dev module) | B0D8T53CQ5 |
| J3 | ALAMSCN black VL53L0X-V2 breakout | B0DGL2P6XC |
| J4 | HiLetgo GY-SHT31-D breakout | B07ZSZW92J |

Each module mates directly with the carrier's female SSW sockets using
its own male pins — not a separately selected or premium replacement
part in any case, but the two modules arrive different ways: the ALAMSCN
VL53L0X-V2 and HiLetgo GY-SHT31-D breakouts (J3, J4) ship with loose
2.54 mm male header strips that the user solders onto the breakout
before plugging it in; the ELEGOO EL-SM-012 (J1, J2) instead plugs in
directly using its own existing male pin rows, already fixed to the
module — nothing to solder there. U1 (silkscreen reference for the ESP32
module's outline) is a mechanical-only footprint with zero pads — it
exists purely
so the module's footprint is visible on the silkscreen/assembly drawing
for placement reference; it is explicitly marked `in_bom=no`, `dnp=yes`
and will not appear in any BOM export.

## Package contents

- `gerbers/` — RS-274X Gerbers: `F_Cu`, `B_Cu`, `F_Mask`, `B_Mask`,
  `F_Silkscreen`, `B_Silkscreen`, `Edge_Cuts`, plus the `.gbrjob` job file.
  No paste layers (this board has no SMD pads — every part is
  through-hole).
- `drill/` — Excellon drill files, PTH and NPTH generated separately:
  `rev-a-carrier-PTH.drl` (55 holes: 15× Ø0.3 mm signal/detour vias, 40×
  Ø1.0 mm component pads across J1/J2/J3/J4), `rev-a-carrier-NPTH.drl` (5
  holes: 3× Ø3.2 mm carrier mounting holes MH1-MH3, 2× Ø3.2 mm VL53L0X
  retention holes at J3). Drill maps (PDF) and a text report included for
  cross-check.
- `bom/RevA_assembly_BOM.csv` — final assembly BOM, grouped by MPN (see
  table above).
- `position/RevA_placement.csv` — placement/position file (X/Y/rotation,
  mm, both sides), DNP items excluded.
- `fab-drawing/RevA_fabrication_drawing.svg` — board outline, mounting
  hole positions, and top-side silkscreen/fab reference text, fitted to
  the board's own ~104×104 mm extent.
- `RevA_Gerbers_NextPCB.zip` — `gerbers/` + `drill/` zipped together,
  ready to upload as-is to NextPCB's quote/order tool.

## Design notes relevant to fabrication

- **2-layer board**, Ø104 mm circular outline with a USB-C access notch
  and an antenna overhang notch cut into the upper hemisphere (see
  `../README.md` "Board outline").
- **VL53L0X socket (J3) is rotated 180° and offset** from the board's
  `LID_ORIGIN` — this is intentional (Rev 7 optical-alignment fix, see
  `../mechanical-stackup.md` §1.3), not a placement error. Mounting holes
  at `(±10, -2.2)` mm relative to `LID_ORIGIN`.
- **No SHT31 retention hole on the carrier** — retention is provided by
  the lid/enclosure (pocket-clip), not a PCB feature. This is intentional.
- Minimum via-to-via hole spacing: 0.7 mm edge-to-edge, comfortably above
  NextPCB's published 0.20 mm same-net via-to-via hole clearance minimum.
- No controlled impedance, no blind/buried vias, no HDI features — this
  is a conventional 2-layer prototype board.

## Not included

Lid/enclosure CAD is not part of this package — it isn't designed yet.
Nothing in this PCB package depends on it; see `../mechanical-stackup.md`
for the parametric lid interface (optical bore, vent pocket, standoff
heights) the eventual lid will need to satisfy.
