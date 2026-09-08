# Rev A Mechanical Stack-up — Sensor Interfaces

Status: **Rev A PCB-affecting mechanical decisions are finalized.** The
VL53L0X optical XY alignment, SHT31 retention approach, and P_sensor
nominal are all settled and reflected in the fabricated copper (§1, §2,
§3). Lid thickness and enclosure geometry remain **parametric** — every
dimension that depends on the lid is still expressed as a formula, not a
fixed number, since the lid itself is not yet designed. Gerbers are
generated this pass; lid CAD is not.

**Hardware identities (established outside this task — used as-is, not
re-researched here):**

| Module | Exact part | Amazon ASIN |
| --- | --- | --- |
| ESP32 | ELEGOO EL-SM-012 | B0D8T53CQ5 |
| VL53L0X | ALAMSCN black VL53L0X-V2 | B0DGL2P6XC |
| SHT31 | HiLetgo GY-SHT31-D | B07ZSZW92J |

**Architecture note — removable plug-in modules, not soldered to the
carrier.** ESP32, VL53L0X, and SHT31 are all plug-in modules: female
Samtec SSW sockets are soldered to the carrier PCB (J1 + J2:
`SSW-115-01-T-S`, 1x15 each, one per ESP32 pin row; J3: `SSW-106-01-T-S`,
1x6; J4: `SSW-104-01-T-S`, 1x4 — 2.54 mm pitch, single-row, through-hole,
lead-free solderable). U1 itself is the ESP32 module's mechanical outline
only — zero pads, not a BOM/assembly item; the module is user-installed,
plugging into J1/J2, not procured or soldered by the assembly house. The
sensor-side male pins are the loose 2.54 mm
header strips already supplied with the exact ALAMSCN VL53L0X-V2 and
HiLetgo GY-SHT31-D modules — not a separately selected or premium
replacement part. Each breakout plugs into its socket and is additionally
mechanically retained by the M2.5 standoff/screw system (§3.1) — except
SHT31, which uses a lid/enclosure pocket-clip instead (§2.3, Rev 7). No
sensor board is soldered directly to the carrier.

**Rev 2 — corrections applied in that pass:** the VL53L0X optical bore is
no longer a precise Ø3.0 mm hole centered on the mounting-hole midpoint
(§1.5, historical); the Z-stack no longer lets a standoff "independently"
set the sensor board's height while the connector "just tolerates" it
(§3); `G_top`/`H_lid`/`D_far = G_top + H_lid` are defined separately, and
the bore is sized at `D_far`, not `G_top`.

**Rev 3 — corrections applied in that pass:** the mounting-hole midpoint
was no longer called `TOF_ORIGIN` while also being ~2.1 mm from the real
optical axis — three separate datums (`LID_ORIGIN`, `J3_MOUNT_ORIGIN`,
`TOF_ORIGIN`) are now named explicitly (§1.1); `P_sensor` (VL53L0X package
protrusion below the breakout board) was introduced so Standoff B accounts
for it (`Standoff_B_J3 = P_sensor + G_top`, `L_total_J3 = H_conn + t_board
+ P_sensor + G_top`), while `D_far = G_top + H_lid` stays unchanged since
it's already referenced from the optical face directly.

**Rev 4 — corrections applied in this pass:**

1. **Removed the unsupported `H_conn = 2.5 mm` assumption.** That number
   was a "low-profile socket" placeholder invented for lack of a selected
   part. It's gone. `H_conn` is now carried **symbolically** through every
   formula and worked example until the real connector-pair geometry is
   known (§3.0, §3.1).
2. **Named the current preferred carrier-side socket family**: Samtec
   `CES-106-01-G-S` (J3, 1×6) and `CES-104-01-G-S` (J4, 1×4) — 2.54 mm
   pitch, single-row, through-hole, closed-entry, low-profile. Their
   published mechanical drawing gives a **5.08 mm socket body height**.
3. **That body height is explicitly not `H_conn`.** `H_conn` is the
   carrier-PCB-underside-to-breakout-board-top distance *once the actual
   mating male header is fully and correctly seated* — a function of the
   male header's own geometry and insertion depth, not of the socket
   alone. Until a matching male header is selected, `H_conn` stays
   unresolved (§3.1).
4. Made every dependent Z-stack formula parametric in `H_conn` rather
   than substituting a specific value for it.

**Rev 5 — correction applied in this pass:**

5. **Removed the intentionally-undersized `Standoff A = H_conn − c`
   model.** A rigid M2.5-screw-clamped standoff cut deliberately shorter
   than the connector's own mated height isn't a tolerance mechanism —
   tightening the screw would have to flex a PCB, preload the connector's
   pins, or leave the stack not actually clamped, to close that gap.
   `Standoff_A_nominal = H_conn` now — the connector establishes the
   seated Z position, and the mechanical support nominally meets the
   breakout at that same plane, no built-in offset. Real tolerance
   accommodation (spacer tolerancing, a compliant washer, shoulder-style
   retention, a rocking-only clip, or similar) is left as an open
   mechanical-detail decision, not invented here (§3.1).
6. **Corrected the framing of the remaining connector task.** The sensor
   breakouts already carry standard 2.54 mm male header interfaces — the
   HiLetgo GY-SHT31-D ships with its own male header strip soldered to the
   board, and the VL53L0X breakout's footprint already reflects the same
   kind of standard header pins. There is no header *family* left to
   choose. What remains is establishing the **actual mated geometry**
   (and the resulting `H_conn`) between those standard pins and the
   carrier-side socket (§3.1).

**Rev 6 — connector decision finalized, this pass:**

7. **Selected carrier-side sockets: Samtec SSW, not CES.** The Rev 4/5
   Samtec CES consideration is superseded. The selected parts are
   `SSW-115-01-T-S` (J1 + J2, 1x15 each, qty 2 — one per ESP32 pin row;
   U1 itself is the ESP32 module's mechanical outline, not a socket),
   `SSW-106-01-T-S` (J3, 1x6), and `SSW-104-01-T-S` (J4, 1x4) — 2.54 mm
   pitch, single-row, through-hole, lead-free solderable (`-S` row
   option). Sensor-side male pins are the loose header strips already
   supplied with the ALAMSCN VL53L0X-V2 and HiLetgo GY-SHT31-D modules —
   no replacement premium header, per instruction, unless a real fit
   problem is found.
8. **`H_conn` is resolved: `H_conn_nominal = 11.05 mm`** for the selected
   SSW stack (J3/J4). This number comes from the connector-stack decision
   made outside this document, not re-derived here. `Standoff_A_nominal =
   H_conn = 11.05 mm` (§3.1) — no longer symbolic. Everything downstream
   (`Standoff_B_J3`, `L_total_J3`) is now a concrete worked example, not
   an illustrative placeholder.
9. **KiCad footprints checked against the Samtec SSW/SSQ published
   mechanical drawing** (2.54 mm pitch; body length = `2.54mm x N +
   0.51mm`) and against KiCad's own verified standard 2.54 mm THT
   pin-socket land pattern (1.0 mm drill). The existing generic J3 and J4
   footprints were found dimensionally correct as-is — reused, not
   replaced. The former combined `ESP32_EL_SM_012_CARRIER_2x15` footprint
   (30 pads, doubling as both the ESP32 module's mechanical outline and
   its two real carrier sockets) was subsequently split — in the
   follow-on BOM-model cleanup, not re-derived here — into
   `ESP32_EL_SM_012_MODULE_OUTLINE` (U1: mechanical-only, zero pads, not
   a BOM item) plus two placements of a new `SSW-115-01-T-S_1x15`
   footprint (J1, J2: the real carrier sockets), at the exact same pad
   coordinates the combined footprint used. See
   `RevA_Sensors.pretty/*.kicad_mod` `descr` fields and the PCB build for
   the specific numbers checked, and the project README's "BOM model"
   section for the full J1/J2/U1 rationale.

**Rev 7 — Rev A PCB-affecting decisions finalized, this pass:**

10. **VL53L0X optical XY alignment resolved: `TOF_ORIGIN = LID_ORIGIN` by
    construction, not by hoping the offset is small.** The Rev A nominal
    optical offset is set to **2.2 mm** (the ALAMSCN listing-photo
    measurement of 2.12 mm, §1.2, corroborated by an independent direct
    measurement of a common GY-VL53L0XV2 board). Rather than shifting J3
    by +2.2 mm in Y — which would have moved it *toward* J1 and eaten
    into the ~1.7 mm J1/J3 courtyard clearance that existed at the time —
    J3 is **rotated 180°** and placed so its mounting-hole midpoint moves
    to `Y = -2.2 mm`, landing the *optical* axis on `LID_ORIGIN` instead.
    Footprint origin `(10, -2.2)`, rotation `180°`. Verified against
    KiCad's own applied transform, not just hand-derived (§1.3): resulting
    `J3_MOUNT_ORIGIN = (0, -2.2)`, `TOF_ORIGIN = (0, ~0) = LID_ORIGIN`,
    mounting holes at `(+10, -2.2)` and `(-10, -2.2)`. A side effect of
    the rotation (not just the shift) is that J3's courtyard now sits
    *farther* from J1 than before (§3.2/README), not closer — the
    original concern is fully addressed, not just avoided.
11. **SHT31 retention resolved: lid/enclosure pocket-clip, not a PCB
    hole.** Rev A does not add a carrier PCB retention hole for J4; the
    footprint's `PRELIM: VERIFY RETENTION HOLE` fab annotation and its
    associated candidate-hole graphic are removed (they no longer apply
    and would misleadingly imply PCB fabrication depends on them). J4's
    board envelope and socket geometry are otherwise unchanged (§2.3).
12. **`P_sensor` finalized as a Rev A nominal, not a PCB hold.**
    `P_sensor_nominal = 1.0 mm` (ST's published VL53L0X package height —
    the package is `4.4 × 2.4 × 1.0 mm`). Normal solder/assembly
    tolerance around this value is an enclosure/standoff tolerance
    question, not something that affects Rev A copper or placement.
13. **Breakout PCB thickness (`t_board`) reclassified as an
    enclosure-fit parameter, not a PCB hold** — it was never part of Rev A
    copper or placement, only of the (still-parametric, still-undesigned)
    lid/standoff stack-up.
14. **The PCB fabrication-hold list is now empty.** See "Remaining
    fabrication holds" below — everything that used to block Gerber
    generation is resolved. What's left are enclosure/mechanical
    fit-check items that the lid design will eventually want, none of
    which hold this PCB.

## 1. VL53L0X-V2 (J3) — optical package location

### 1.1 Datums — three separate points, now two of them coincide by design

This design has three distinct reference points that must not be
conflated, even though (as of Rev 7) two of them are now deliberately
made to coincide:

| Datum | Meaning | Current value | Status |
| --- | --- | --- | --- |
| **`LID_ORIGIN`** | The intended measurement point over the dough — the fixed reference everything else is judged against. | `(0, 0)` | Fixed, by definition — doesn't move. |
| **`J3_MOUNT_ORIGIN`** | Midpoint of J3's two M2.5 retention holes (20.0 mm apart). | `(0, -2.2)` | **Fixed by the Rev 7 footprint placement** (origin `(10,-2.2)`, rotation `180°`). No longer coincident with `LID_ORIGIN` — that trade was made deliberately so the *optical* axis, not the hole-midpoint, lands on the datum that actually matters. |
| **`TOF_ORIGIN`** | The VL53L0X's actual optical (emitter/receiver) axis. | `≈ (0, 0) = LID_ORIGIN` | **Achieved by construction (Rev 7)**, using the 2.2 mm Rev A nominal offset (§1.2/§1.3). Nominal, not caliper-verified against the exact physical part in hand — but no longer a PCB-blocking hold; residual error, if any, is well within the lid opening's built-in margin (§1.4). |

**Rev A achieved state** (verified against KiCad's own applied
position+rotation transform, not just hand-derived — §1.3):

```
J3_MOUNT_ORIGIN = (0, -2.2)          -- no longer at LID_ORIGIN
TOF_ORIGIN      = (0, ~0) = LID_ORIGIN   -- this is what's aligned now
```

Sections 1.4 onward say `TOF_ORIGIN` when they mean the optical axis and
`LID_ORIGIN`/`J3_MOUNT_ORIGIN` when they mean the fixed measurement point
or the hole geometry — the three are still not interchangeable, even
though `TOF_ORIGIN` and `LID_ORIGIN` now coincide.

- **Fixed by the footprint (exact):** the two M2.5 retention holes are
  20.0 mm apart. As of Rev 7 their midpoint (`J3_MOUNT_ORIGIN`) sits at
  `(0, -2.2)`, not at `LID_ORIGIN`. The two-hole rigid retention itself
  (geometry, hole size) is unchanged — only where it's placed moved.
- **Measured (§1.2):** the VL53L0X module's package outline position
  relative to those two holes, from the ALAMSCN listing's own product
  photography — this is what located `TOF_ORIGIN` and set the 2.2 mm Rev
  A nominal.
- **Still nominal, not caliper-verified:** the exact optical
  (emitter/receiver) centroid *within* the package outline. ST's VL53L0X
  datasheet gives the package size precisely (4.40 x 2.40 x 1.00 mm,
  "Optical LGA12") but does not publish the emitter/collector centroid
  offset from the package edge as a text-extractable dimension. Treating
  the package's geometric center as `TOF_ORIGIN` is a reasonable
  approximation, corroborated by an independent measurement of a common
  GY-VL53L0XV2 board — good enough confidence to commit to fabrication,
  not the same as a caliper/microscope measurement of the exact part in
  hand.

### 1.2 Measured offset from the ALAMSCN listing photos

The ALAMSCN listing (ASIN B0DGL2P6XC) product photography includes a
straight-on shot of the sensor-chip side of the board, silkscreened
`VL53L0/1XV2`, with a white rectangle marking the sensor module's own
footprint. Both mounting holes and the rectangle are clearly resolved.
Measuring directly from that image (hole spacing = 20.0 mm used as the
pixel-to-mm scale reference):

| Quantity | Value |
| --- | ---: |
| Measured module outline size | 4.92 x 2.83 mm (vs. ST's nominal 4.40 x 2.40 mm package — consistent, silkscreen courtyard is drawn slightly oversized) |
| `TOF_ORIGIN`, X, relative to `J3_MOUNT_ORIGIN` (pre-Rev-7 hole placement) | **-0.09 mm** (i.e. centered — within measurement noise) |
| `TOF_ORIGIN`, Y, relative to `J3_MOUNT_ORIGIN` (pre-Rev-7 hole placement) | **-2.12 mm** (offset away from the 6-pin header, toward the "blank" end of the board) |

In the footprint's own local frame (holes at local (0,0)/(20,0), header
pins at local Y=+5.35): `TOF_ORIGIN` sits at approximately local
**(10, -2.1)** — i.e. right at `J3_MOUNT_ORIGIN` in X, and about 2.1 mm
toward the header-free end in Y. **Rev A nominal, used for the placement
in §1.3, rounds this to 2.2 mm**, corroborated by an independent direct
measurement of a common GY-VL53L0XV2 board (not the exact ALAMSCN part,
but close geometry, done outside this document).

**Confidence:** this is measured from the exact listing's photography
(not a generic proxy board), so it's a real, specific data point — but it
is still a photo measurement, not a caliper or microscope measurement of
the physical part. Two independent sources now agree closely enough to
commit to fabrication (Rev 7) — see the fabrication-holds list for the
residual, non-blocking follow-up.

### 1.3 Rev A implementation — J3 rotated 180°, `TOF_ORIGIN` on `LID_ORIGIN`

**The problem with the obvious fix.** The naive correction — shift J3 by
+2.2 mm in Y, leaving its orientation alone — moves `J3_MOUNT_ORIGIN` (and
therefore J3's whole footprint, including its courtyard) *north*, i.e.
*toward* J1 (the ESP32 carrier socket immediately above it). At the time
this decision was made the J1/J3 courtyard gap was only ~1.7 mm; a
straight +2.2 mm shift would have consumed all of that margin and then
some. **Rejected for that reason.**

**The Rev 7 fix: rotate 180°, don't just translate.** The footprint's
local geometry is fixed: mounting holes at local `(0,0)` and `(20,0)`,
`J3_MOUNT_ORIGIN` at local `(10,0)`, optical package (`TOF_ORIGIN`) at
local `(10,-2.2)` (§1.2's measurement, rounded to the Rev A nominal).
Placing the footprint origin (anchor = local `(0,0)`) at global `(10,
-2.2)` with a `180°` rotation:

```
rotated_local(x,y) = (-x,-y)                     [180° about the anchor]
global(p)          = anchor + rotated_local(p)

J3_MOUNT_ORIGIN  (local 10,0):    (10,-2.2) + (-10, 0)  = (0, -2.2)
TOF_ORIGIN       (local 10,-2.2): (10,-2.2) + (-10, 2.2) = (0, 0)  = LID_ORIGIN
hole 1 (local 0,0):               (10,-2.2) + (0, 0)     = (10, -2.2)
hole 2 (local 20,0):              (10,-2.2) + (-20, 0)   = (-10, -2.2)
```

**Verified against KiCad's own applied transform after placement, not
just this hand derivation** — independently reconstructing pcbnew's
actual position+orientation transform and mapping the same local points
through it:

```
placed footprint anchor:  (10.0, -2.2) mm, orientation 180.0°
hole 1 (actual pad):      (10.0, -2.2) mm
hole 2 (actual pad):      (-10.0, -2.2) mm
J3_MOUNT_ORIGIN (midpoint of actual holes): (0.0, -2.2) mm
TOF_ORIGIN (local (10,-2.2) mapped through the SAME transform): (0.0, ~0.0) mm
```

The optical axis lands on `LID_ORIGIN` to floating-point precision. A
useful side effect of rotating (not just translating): J3's courtyard
shape is now oriented the *other* way too, so the net result is J3 moved
*away* from J1 overall (courtyard gap grew, §3.2/README), not toward it —
the original concern that ruled out the naive shift doesn't apply to the
rotation.

**Pin mapping is unaffected by the rotation.** J3's pin-to-net assignment
is a schematic-level fact, untouched by this PCB-only placement change:
pin 1 = 3V3, pin 2 = GND, pin 3 = SCL/GPIO21, pin 4 = SDA/GPIO22, pin 5
(GPIO1) and pin 6 (XSHUT) = NC. Only the physical *location* of those pins
moved (and, because of the 180° rotation, their X-ordering reversed and
their row moved from the north side of the mount holes to the south side)
— routing was rerouted to reach the new positions (README "Routing"), but
no net assignment changed.

### 1.4 Rev A final lid opening geometry

Three separate lengths matter here, and they must not be conflated:

- **`G_top`** — air gap from the sensor's optical face (`TOF_ORIGIN`)
  down to the lid's **top** (near, sensor-side) surface.
- **`H_lid`** — lid thickness (parametric, independent material choice).
- **`D_far = G_top + H_lid`** — distance from `TOF_ORIGIN` down to the
  lid's **chamber-side (far)** surface.

VL53L0X full field of view is **25°** (ST datasheet: "VL53L0X system FOV
is 25 degrees" / half-angle 12.5°), unchanged by cover glass choice (see
1.6). For a straight cylindrical bore of constant diameter, the FOV cone
is widest at the **far** end, so `D_far` — not `G_top` — is what must size
the bore:

```
opening_diameter(H_lid) = 2 · D_far · tan(12.5°) + margin
                         = 2 · (G_top + H_lid) · tan(12.5°) + margin
```

using `G_top = 1.5 mm` (target) and `margin = 1.0 mm` total:

| H_lid | D_far = G_top + H_lid | Bore Ø (centered exactly on `LID_ORIGIN`, = `TOF_ORIGIN` as of Rev 7) |
| ---: | ---: | ---: |
| 2.0 mm | 3.5 mm | 2.6 mm |
| **3.0 mm** | **4.5 mm** | **3.0 mm** |
| 4.0 mm | 5.5 mm | 3.4 mm |
| 5.0 mm | 6.5 mm | 3.9 mm |

**This is now the live spec, not a conditional future one.** Because J3
is physically rotated/repositioned so `TOF_ORIGIN = LID_ORIGIN` (§1.3),
this bore can be cut centered on `LID_ORIGIN` directly — the earlier
"provisional oversized opening, offset from `LID_ORIGIN`" approach (old
§1.5, kept below as historical record) is superseded and no longer
needed. This table does not depend on `H_conn` at all — the optical-bore
sizing and the connector question are independent.

**Recommendation: minimize `G_top`.** A small `G_top` keeps `D_far` (and
therefore the bore) small — good for a clean chamber-facing surface, a
minimal steam/condensate ingress path, and a short response time. Target
**`G_top` = 1.5 mm**.

### 1.5 Historical: the pre-Rev-7 provisional opening (superseded)

Before Rev 7, the PCB kept `J3_MOUNT_ORIGIN = LID_ORIGIN` and `TOF_ORIGIN`
was ~2.1-2.2 mm away, unresolved. Because a final-spec bore's own radius
(~1.5 mm) was smaller than that offset, a precision bore centered on
`LID_ORIGIN` would have risked missing the sensor's real field of view —
so two placeholder options were defined for a first fit-check lid only:
**Option A**, an oversized (~Ø5.0 mm at `H_lid`=3mm) opening centered on
the *estimated* `TOF_ORIGIN` rather than `LID_ORIGIN`; **Option B**, a
fully direction-agnostic Ø9.5 mm opening centered on `LID_ORIGIN` for a
team unwilling to trust even the measured offset's direction. **Both are
superseded by Rev 7** (§1.3/1.4) — `TOF_ORIGIN` now *is* `LID_ORIGIN` by
construction, so the final-spec bore (§1.4) applies directly. Kept here
only as a record of the reasoning that preceded the Rev 7 fix.

### 1.6 Cover glass — avoid unless necessary

Per the brief, default to **no cover** over the VL53L0X opening — a bare
through-hole, open to the chamber:

- **Ranging offset shift.** ST's guidance: a cover window changes the
  ranging offset and needs recalibration.
- **Cross-talk.** A cover glass reflects some VCSEL output back into the
  receiver, reducing usable range.

**When a cover would become necessary:** if condensation or flour dust
ingress into the electronics bay turns out to be a real problem. If so,
the fix is a thin (0.5-1 mm) IR-transparent window bonded at the lid's
chamber-side surface, sized to the bore diameter from 1.4, with the
offset/cross-talk recalibration noted as a required follow-up. Open
decision, not resolved here.

## 2. SHT31 (J4) — ventilated pocket, lid/enclosure retention

J4 is the exact **HiLetgo GY-SHT31-D (ASIN B07ZSZW92J)** established for
this project — not re-researched or substituted in this pass.

### 2.1 Location — unchanged

J4 stays at its established placement (footprint origin / pin 1 / VIN at
**(0, -29)** relative to `LID_ORIGIN`), clear of the VL53L0X optical path
and clear of J3's hardware by a de-risked margin (recomputed against the
Rev 7 J3 placement — see README "Mechanical: top vs. bottom overlap" for
the current J3/J4 clearance number). No placement change in this pass.

### 2.2 Ventilated pocket concept

The SHT31's sensing element is a capacitive RH + temperature die on the
underside of the small breakout board — unlike the VL53L0X, it has no
aiming requirement, just needs representative chamber air to reach it
without pooling condensate on the die. Proposed pocket, sized to the
board's real outline (10.7 x 13.1 mm per the existing footprint
annotation):

- A shallow rectangular **recess** in the lid's chamber-side face, sized
  to the board's real outline plus ~0.5 mm clearance per side, deep enough
  to seat the board's retention feature (see 2.3) flush or slightly
  recessed — not proud of the lid.
- **Vent pattern**, not a single large opening: 6-8x Ø1.5 mm holes
  positioned under the sensing-element half of the board, clear of the
  retention feature.
- **Orientation:** vent pattern on the chamber-facing surface, not the
  side — avoids being a direct drip target.
- Multiple small holes over one big opening: keeps any single opening too
  small for a condensate droplet to bridge, still gives enough open area
  for reasonable RH response, and leaves most of the recess floor solid
  for retention.

### 2.3 Mechanical retention — resolved (Rev 7): lid/enclosure pocket-clip

**Rev A uses the hard lid pocket/clip retention option, not the
breakout's own mounting hole.** This closes what was previously an open
PCB-adjacent fabrication hold:

- **No carrier PCB retention hole for J4.** None is added in Rev A, and
  none was ever strictly required for this option — the retention feature
  lives entirely in the lid/enclosure.
- **The footprint's `PRELIM: VERIFY RETENTION HOLE` annotation and its
  candidate-hole graphic are removed** (Rev 7) — they described a design
  path Rev A isn't taking, and left in place would misleadingly suggest
  PCB fabrication depended on resolving them. The footprint now carries a
  `RETENTION: LID/ENCLOSURE POCKET-CLIP (NOT A PCB HOLE)` note instead.
- **J4's board envelope and socket geometry are unchanged** — the pocket
  concept from §2.2 (recess sized to the board outline + clearance, vent
  pattern in the floor) still applies; only the *retention* mechanism
  (tabs/ribs molded into the lid, gripping the board's edges) is now
  settled, per §3.1's same Z-stack model (the tabs are positioned at the
  connector's mated height, `H_conn`, same constraint a standoff would
  have had).
- **This is now an enclosure/lid-CAD item, not a PCB item.** The exact
  tab/rib geometry is still undesigned (lid CAD hasn't started — see
  status line at top), but nothing about it requires a carrier PCB
  change, so it does not hold Gerber generation.

(SHT31's own sensing die is assumed to sit at the breakout board's
underside — no separate `P_sensor`-style package protrusion is modeled
for it; low stakes given the retention mechanism no longer depends on
board-hole registration.)

## 3. Full stack-up (top/outside to bottom/inside)

All Z dimensions are relative to the carrier PCB's own top surface (Z=0),
positive Z upward, negative Z downward (toward the lid/chamber side). See
§1.1 for the `LID_ORIGIN`/`J3_MOUNT_ORIGIN`/`TOF_ORIGIN` datums.

### 3.0 Key parameters

| Symbol | Meaning | Status |
| --- | --- | --- |
| `t_carrier` | Carrier PCB thickness | Fixed: 1.6 mm (existing PCB) |
| `H_conn` | Female socket → mated standard male header → breakout board's top surface, fully and correctly seated: carrier PCB underside to breakout board top | **Resolved (Rev 6): `H_conn_nominal = 11.05 mm`.** Carrier-side socket selected — Samtec `SSW-106-01-T-S` (J3, 1×6) / `SSW-104-01-T-S` (J4, 1×4), 2.54 mm pitch, single-row, THT, lead-free solderable. Mated with the loose male header strips already supplied with the ALAMSCN and HiLetgo modules. `H_conn_nominal` comes from the connector-stack decision made outside this document. |
| `t_board` | Sensor breakout board thickness (J3, J4) | **Enclosure-fit parameter (Rev 7), not a PCB hold** — doesn't affect Rev A copper or placement, only the (still-undesigned) lid/standoff stack-up. Provisional value: 1.3 mm (typical small 2-layer FR4), assumed equal for both, unmeasured. |
| `P_sensor` | VL53L0X package protrusion: breakout board underside → optical face (`TOF_ORIGIN`) | **Resolved (Rev 7): `P_sensor_nominal = 1.0 mm`** — ST's published Optical LGA12 package height (package `4.4 × 2.4 × 1.0 mm`). Normal solder/assembly tolerance around this value is an enclosure/standoff tolerance question, not a PCB hold. |
| `G_top` | Air gap, `TOF_ORIGIN` → lid top (near) surface | **Design target**, achieved by the lid-facing standoff segment (§3.1). Target: 1.5 mm |
| `G_vent` | Equivalent gap for SHT31 | **Derived**, not independently targeted, once `G_top` fixes the shared lid plane (§3.1) |
| `H_lid` | Lid thickness | Free/parametric, independent material choice |
| `D_far = G_top + H_lid` | `TOF_ORIGIN` → lid chamber-side (far) surface | Derived — sizes the optical bore (§1.4). Does **not** depend on `H_conn`. |
| `L_total_J3 = H_conn + t_board + P_sensor + G_top` | Carrier PCB underside → lid top surface, at J3 | Derived: `11.05 + 1.3 + 1.0 + 1.5 = 14.85 mm` (illustrative — `t_board` is still an unmeasured enclosure-fit value; `H_conn` and `P_sensor` are now both resolved nominals; `G_top` is a design target) — see §3.1 |

### 3.1 Standoffs are two stacked segments per sensor, not one free length

**Plug-in module architecture.** Each sensor is a plug-in module: a Samtec
SSW female socket is soldered to the carrier PCB; the breakout module's
own male header pins plug into it. The female header and the mated male
pins fix the board's Z position once seated — a standoff cannot
independently choose a different height while the connector "just
tolerates" it. The fix, carried over from Rev 2, is two stacked standoff
segments per sensor on the same M2.5 screw path already used for the
two-hole rigid retention, with the sensor board sandwiched between them
(for J4, as of Rev 7, this describes the lid pocket-clip's tabs, not a
standoff/screw — same Z-constraint, different hardware, §2.3):

- **Standoff A** (carrier PCB underside → breakout board top surface):
  nominal length `Standoff_A_nominal = H_conn = 11.05 mm`. The connector
  establishes the seated Z position; the mechanical support meets the
  breakout at that same plane, with no built-in offset. An
  intentionally-undersized rigid standoff was considered and rejected
  (Rev 5) — on a rigid M2.5-screw clamp, cutting the standoff deliberately
  shorter than `H_conn` doesn't create a working tolerance mechanism, it
  just forces the screw to close that gap by flexing a PCB, preloading the
  connector's own pins, or leaving the stack not actually clamped. **Real
  tolerance accommodation is a separate, still-open decision** — resolving
  `H_conn` doesn't resolve this too — once real parts are in hand, options
  worth considering include a spacer machined to a tighter matched
  tolerance, a compliant washer that absorbs small stack-up variation
  without applying continuous Z preload to the header pins, shoulder-style
  retention, or a clip that constrains rocking without clamping the boards
  rigidly together along Z. None of these is selected here — this is an
  **enclosure/mechanical fit-check item (Rev 7 reclassification)**, not a
  PCB fabrication hold (see the closing list below).
- **Standoff B** (breakout board underside → lid top surface): length
  `P_sensor + G_top = 2.5 mm`. ONE physical spacer spanning from the
  board's underside to the lid; `TOF_ORIGIN` sits partway along that run,
  `P_sensor` below the board. This segment never depended on `H_conn`.

**Connector: Samtec SSW selected, `H_conn_nominal = 11.05 mm`.** The
carrier-side socket is **Samtec `SSW-106-01-T-S`** (J3, 1×6) and
**`SSW-104-01-T-S`** (J4, 1×4) — 2.54 mm pitch, single-row, through-hole,
lead-free solderable (`-S` row option). The mating male pins are the loose
2.54 mm header strips already supplied with the ALAMSCN VL53L0X-V2 and
HiLetgo GY-SHT31-D breakouts — not a separately selected or premium
replacement part, per instruction, unless a real fit problem turns up.
`H_conn_nominal = 11.05 mm` is the resolved carrier-PCB-underside-to-
breakout-board-top distance for this selected SSW stack; it comes from the
connector-stack decision made outside this document, not re-derived here.

**Worked example — concrete, using the resolved `H_conn` and `P_sensor`.**
Known values: `H_conn = 11.05 mm` (resolved), `t_board = 1.3 mm`
(enclosure-fit, unmeasured), `P_sensor = 1.0 mm` (resolved nominal),
`G_top = 1.5 mm` (target). `H_lid` shown at 3 mm for illustration only —
still free/parametric.

```
Standoff A (nominal)   = H_conn                  = 11.05 mm
Standoff B              = P_sensor + G_top        = 2.5 mm
L_total_J3               = H_conn + t_board + P_sensor + G_top
                         = 11.05 + 1.3 + 1.0 + 1.5 = 14.85 mm
D_far (at H_lid=3 mm)   = G_top + H_lid            = 4.5 mm     (still no H_conn dependency)
```

```
                                          Z (mm, carrier PCB top = 0)
  ═══════════════════════════════   Z =   0.00       carrier PCB top
  ┃▓▓▓▓▓▓▓▓▓▓ carrier PCB ▓▓▓▓▓┃
  ═══════════════════════════════   Z =  −1.60        carrier PCB bottom (t_carrier)
       ┊ Standoff A (nominal)    ┊  Z =  −1.60 .. −12.65   support only —
       ┊ = H_conn = 11.05 mm     ┊              meets the breakout at the SAME
       ┊ (tolerance accommodation┊              plane the connector seats it at,
       ┊  left open — see text)  ┊              not an intentional undersize
  - - -│- - - - - - - - - - - - -│- - -  Z = −12.65        breakout board TOP
       │ VL53L0X breakout module,│       (fixed by the connector, once mated —
       │ plugged into socket,    │       H_conn = 11.05 mm, resolved)
       │ t_board = 1.3 mm        │  Z = −12.65 .. −13.95
  - - -│- - - - - - - - - - - - -│- - -  Z = −13.95        board UNDERSIDE
       ┊ Standoff B (free,       ┊  Z = −13.95 .. −16.45
       ┊  no H_conn dependency)  ┊              length = P_sensor+G_top = 2.5 mm
       ┊ = P_sensor + G_top      ┊
  · · ·│· · package (P_sensor) ·│· · ·  Z = −14.95        TOF_ORIGIN (optical
       │· · · · · G_top · · · · ·│               face) — inside the Standoff B
       │                         │               run, NOT at the board underside
  ┌────┴─────────────────────────┴────┐
  │              LID  (H_lid)          │  Z = −16.45 .. −19.45  (H_lid=3mm shown
  └────┬─────────────────────────┬────┘             for illustration — free param)
  ~ ~ ~│~ ~ ~ chamber interior ~~│~ ~ ~   Z < −19.45, proofing chamber
```

`D_far` (`TOF_ORIGIN` → lid far surface) = `−14.95 → −19.45 = 4.5 mm =
G_top + H_lid`, matching §1.4 exactly — confirms the bore-sizing math and
the now-concrete standoff math agree. This Z-stack is unaffected by J3's
Rev 7 XY rotation — that change only moved J3's mounting-hole/optical
position in the board plane, not any Z dimension.

**`t_board` is still an unmeasured enclosure-fit value — `H_conn` and
`P_sensor` are now both resolved nominals.** The Z numbers above are a
concrete worked example for the selected connector stack and Rev A
optical nominal, not a fully frozen enclosure design: `t_board` (real
breakout thickness) still needs physical measurement for a tight lid fit
(see the closing list), and `G_top` is a design target, not yet locked to
a specific lid. Separately, how real manufacturing tolerance on the
seated stack gets accommodated (a matched-tolerance spacer, a compliant
washer, shoulder-style retention, a rocking-only clip, or similar) is an
open mechanical-detail decision, not resolved by `H_conn_nominal` — it
isn't solved by adjusting Standoff A's nominal length. None of this holds
Gerber generation.

**Same lid plane serves both sensors.** The lid is one flat plate, so
whatever sets `L_total_J3` applies to the whole lid, including under J4.
J3 and J4 use the same Samtec SSW family (same `-01-T-S` lead style /
row option, differing only in pin count), so `H_conn` is the same
resolved 11.05 mm for both. Assuming both breakout boards share the same
provisional `t_board`:

```
G_vent = L_total_J3 − H_conn_J4 − t_board_J4
       = 14.85 − 11.05 − 1.3
       = 2.5 mm
```

— larger than the earlier informal 0.5-1 mm target, because the shared
lid plane is set relative to the VL53L0X's optical face, one `P_sensor`
further from the board than the SHT31 side. Still workable for a sensor
with no FOV requirement. If SHT31's real board thickness turns out to
differ from J3's once both are physically measured, `G_vent` would need
to be recomputed directly rather than assumed equal.

### 3.2 Cutouts required in the lid

1. **VL53L0X optical bore**: **Ø3.0 mm** (at `H_lid` = 3 mm; recompute via
   §1.4's formula for a different lid thickness), centered exactly on
   `LID_ORIGIN` — which, as of Rev 7, *is* `TOF_ORIGIN` by construction
   (§1.3), so no offset-centering or oversizing is needed anymore.
   Straight through the full lid thickness, axis normal to the lid.
2. **VL53L0X standoff clearance**: two Ø3.2-4 mm clearance holes at
   **`(-10, -2.2)` and `(+10, -2.2)`** — `J3_MOUNT_ORIGIN`-referenced,
   moved by Rev 7's J3 rotation (previously `(-10, 0)`/`(+10, 0)`) — each
   carrying a stacked Standoff A + Standoff B pair per §3.1. Standoff A =
   `H_conn_nominal` = 11.05 mm (resolved); Standoff B = 2.5 mm. The
   tolerance-accommodation method for Standoff A is an open
   enclosure-fit item (§3.1), not a PCB hold.
3. **SHT31 vent pocket**: rectangular recess sized to ~11.2 x 13.6 mm
   (board outline + 0.5 mm/side clearance), under J4's real board
   position, with the 6-8x Ø1.5 mm vent pattern from 2.2.
4. **SHT31 retention tabs/ribs**: lid-molded pocket-clip features per
   §2.3 (Rev 7), positioned at `H_conn`'s mated height, same constraint a
   standoff would have had. No carrier PCB feature required — this is a
   lid-CAD item only.

The cutout *positions* don't depend on `H_lid` or `H_conn`, except that
cutout 2's exact XY moved with Rev 7's J3 rotation (a one-time PCB-driven
update, already reflected above). The VL53L0X bore *diameter* depends on
`H_lid` (via `D_far`), not on `H_conn`. The standoff *lengths* (§3.1)
depend on `H_lid` (via `L_total_J3`) and, for Standoff A, on the resolved
`H_conn_nominal = 11.05 mm`.

## Remaining fabrication holds

**None.** As of Rev 7, nothing blocks Gerber generation for this PCB — the
VL53L0X optical XY alignment, the SHT31 retention approach, `P_sensor`,
and the connector stack (`H_conn`, socket selection) are all resolved.

### Enclosure / mechanical fit-check items (not PCB holds)

Left open deliberately — none of these require a PCB change, and none
block fabrication, but the lid/enclosure design (not yet started) will
eventually want them resolved:

1. **Breakout PCB thickness** (`t_board`, both J3 and J4, §3.0) —
   currently a generic 1.3 mm small-FR4-board assumption for both,
   unmeasured. Affects lid/standoff fit only.
2. **Final tolerance-accommodation method for Standoff A** (§3.1) — a
   nominal target length (`H_conn_nominal = 11.05 mm`) exists, but how
   real assembly tolerance on that seated stack gets accommodated
   (matched-tolerance spacer, compliant washer, shoulder-style retention,
   rocking-only clip, or similar) is still an open mechanical-detail
   decision, and applies to J4's pocket-clip tabs too (§2.3).
3. **SHT31 pocket-clip tab/rib geometry** (§2.3) — the retention approach
   is decided; the exact tab dimensions are a lid-CAD detail, not started.
4. *(Nice-to-have, not blocking):* physical caliper/microscope
   confirmation of the 2.2 mm `TOF_ORIGIN` nominal (§1.2) and the 1.0 mm
   `P_sensor` nominal (§3.0) against the exact ALAMSCN part, useful for
   optimizing a future tighter lid opening — but the current Ø3.0 mm bore
   already carries a 1.0 mm total alignment margin (§1.4), so a modest
   discrepancy from these nominals doesn't create a hard failure, only a
   less-than-maximally-tight opening.

## References

- ST VL53L0X datasheet (DocID029104 Rev 2), Section 1.1 (package: Optical
  LGA12, 4.40 x 2.40 x 1.00 mm), Section 6 (outline drawing), FOV = 25°.
- ALAMSCN black VL53L0X-V2 listing (ASIN B0DGL2P6XC) product photography —
  source of the measured 1.2 offset; corroborated by an independent direct
  measurement of a common GY-VL53L0XV2 board (Rev 7).
- HiLetgo GY-SHT31-D product listing (ASIN B07ZSZW92J).
- ELEGOO EL-SM-012 product listing (ASIN B0D8T53CQ5).
- Samtec SSW/SSQ series mechanical drawing (Revision B, Nov 2018;
  `suddendocs.samtec.com/catalog_english/ssw_th.pdf`) — 2.54 mm pitch,
  single-row THT, 0.025" sq. post, insertion depth 3.68-6.35 mm, body
  length formula `2.54mm x N + 0.51mm`. Selected carrier parts: J1+J2 2x
  `SSW-115-01-T-S` (1×15, the two ESP32 socket rows), J3 `SSW-106-01-T-S`
  (1×6), J4 `SSW-104-01-T-S` (1×4). Supersedes the Rev 4/5 Samtec CES
  consideration.
- Footprints: `RevA_Sensors.pretty/ESP32_EL_SM_012_MODULE_OUTLINE.kicad_mod`
  (U1: mechanical-only, non-BOM — the ESP32 module itself, zero pads;
  split off the former combined `ESP32_EL_SM_012_CARRIER_2x15` footprint
  in the follow-on BOM-model cleanup),
  `RevA_Sensors.pretty/SSW-115-01-T-S_1x15.kicad_mod` (J1/J2, the two
  carrier-side ESP32 sockets — new, at the split-off footprint's former
  pad coordinates),
  `RevA_Sensors.pretty/VL53L0X_V2_1x06_2H_PRELIM.kicad_mod` (J3 —
  repositioned/rotated in Rev 7, pads/geometry otherwise unchanged),
  `RevA_Sensors.pretty/SHT31_GY-SHT31-D_1x04_PRELIM.kicad_mod` (J4 —
  retention-hole annotation removed in Rev 7, pads/geometry otherwise
  unchanged) — J3 and J4 checked against the Samtec SSW/SSQ drawing above
  and reused unmodified (Rev 6).
