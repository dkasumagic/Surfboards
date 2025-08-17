#!/usr/bin/env python3
import os, json, re
from datetime import date
from pathlib import Path
import skip  # pip install kicad-skip

# ---------- config you can tweak ----------
SEED_SCH = "build/MatMul_seed/MatMul_seed.kicad_sch"
OUT_SCH  = "build/auto.kicad_sch"

# Where your auto-generated symbol lives (from kipart)
# Keep lib:name stable across runs so the seed symbol stays valid
CHIP_LIBID = "MatMulChip"

# Where we place things (KiCad units are mm)
ORIGIN_X, ORIGIN_Y = 100, 100
CHIP_POS           = (ORIGIN_X, ORIGIN_Y)
HEAD_A_POS         = (ORIGIN_X - 60, ORIGIN_Y)     # left of chip
HEAD_B_POS         = (ORIGIN_X + 60, ORIGIN_Y)     # right of chip
HEAD_OUT_POS       = (ORIGIN_X, ORIGIN_Y + 55)     # below chip
POWER_ROW_Y        = ORIGIN_Y - 40
DECOUPLE_RING_R    = 18  # distance from chip center for caps

# What we expect from your yosys design.json
DESIGN_JSON = "build/design.json"

# Optional: which port name prefixes map to which headers
BUS_TO_HEADER = {
    "A": "J_A",
    "B": "J_B",
    "OUT": "J_OUT",
}

# Create output dir
os.makedirs(os.path.dirname(OUT_SCH), exist_ok=True)

# ---------- helpers ----------
def xy(elem):
    """Return (x, y) in schematic units from a skip element with .at.value"""
    v = elem.at.value
    return v[0], v[1]

def sanitize_header_and_title(in_path: str, out_path: str, title: str = "MatMulChip Auto"):
    """
    Fix bad header tokens (e.g., 'generator_...') and ensure a minimal title_block exists.
    Writes sanitized output to out_path.
    """
    text = Path(in_path).read_text(encoding="utf-8", errors="replace")
    start = text.find("(kicad_sch")
    m_paper = re.search(r'\(paper\s+"[^"]+"\)', text)
    keep_from = m_paper.start() if m_paper else (text.find("\n") if "\n" in text else len(text))

    clean_header = '(kicad_sch (version 20240114) (generator "skip") (generator_version "0") '
    new = clean_header + text[keep_from:]

    # --- Ensure title_block exists (insert right after the paper node)
    if "(title_block" not in new:
        if "(paper " in new:
            insert_after = new.find(')', new.find('(paper')) + 1
        else:
            insert_after = new.find('\n') + 1
        tb = f'''
  (title_block
    (title "{title}")
    (company "")
    (rev "")
    (date "{date.today().isoformat()}")
    (comment 1 "")
    (comment 2 "")
    (comment 3 "")
    (comment 4 "")
  )'''
        new = new[:insert_after] + tb + new[insert_after:]
    Path(out_path).write_text(new, encoding="utf-8")

def load_ports_from_yosys(design_json):
    """Return [(name,width,dir), ...] from top module."""
    d = json.load(open(design_json))
    top = next((m for m,v in d["modules"].items() if "ports" in v), None)
    if top is None:
        raise RuntimeError("No module with 'ports' in design.json")
    ports = []
    for name, p in d["modules"][top]["ports"].items():
        direction = p["direction"]
        width = len(p["bits"])
        ports.append((name, width, direction))
    return ports

def expand_bus(name, width):
    if width <= 1: return [name]
    # Make readable MSB..LSB labels like A[7], A[6], ... A[0]
    return [f"{name}[{i}]" for i in range(width-1, -1, -1)]

def next_ref(base, existing):
    """Generate next available reference like J1, J2, ... given existing."""
    nums = [int(m.group(1)) for r in existing
            if r.startswith(base)
            for m in [re.search(r'(\d+)$', r)] if m]
    nxt = (max(nums)+1) if nums else 1
    return f"{base}{nxt}"

def pin_by_name(symbol, raw):
    """
    Find a pin object by its DISPLAY name (e.g., 'A[3]'), regardless of how skip
    python-izes attribute names. Falls back to linear search.
    """
    for p in symbol.pin:
        if getattr(p, "name", None) and p.name == raw:
            return p
    # Some libraries name pins by number only; try numeric match
    # raw like 'A[3]' won't match; the caller should handle that case.
    return None

def place_decouplers(sch, chip, count=4, value="0.1uF"):
    """
    Clone C1 around the chip at 90-degree increments and wire them to VCC/GND nets via labels.
    """
    if len(sch.symbol.reference_matches(r'^C1$')) == 0:
        raise RuntimeError("Seed schematic needs a C1 capacitor to clone.")
    seed_c = sch.symbol.C1
    cx, cy = xy(chip)

    for i in range(count):
        c = seed_c.clone()
        # simple cardinal placement; tweak as you like
        if i == 0:
            c.move(cx - DECOUPLE_RING_R, cy)
        elif i == 1:
            c.move(cx + DECOUPLE_RING_R, cy)
        elif i == 2:
            c.move(cx, cy - DECOUPLE_RING_R)
        else:
            c.move(cx, cy + DECOUPLE_RING_R)

        c.property.Value.value = value

        # label VCC/GND near each cap
        lv = sch.label.new(); lv.value = "VCC"; lv.move(c.at.value[0] - 5, c.at.value[1])
        wv = sch.wire.new();  wv.start_at(lv); wv.end.value = [lv.at.value[0] + 3, lv.at.value[1]]

        lg = sch.label.new(); lg.value = "GND"; lg.move(c.at.value[0] + 5, c.at.value[1])
        wg = sch.wire.new();  wg.start_at(lg); wg.end.value = [lg.at.value[0] - 3, lg.at.value[1]]

def add_power_bundle(sch, chip):
    """
    Drop VCC, GND, PWR_FLAG near the top and label a short bar as VCC/GND nets.
    We find the seed symbols by their Value text (VCC, GND, PWR_FLAG).
    """
    # Find the seed instances by Value (more stable than hardcoding #PWR01 etc.)
    vcc_seed = sch.symbol.value_matches(r'^VCC$')
    gnd_seed = sch.symbol.value_matches(r'^GND$')
    flg_seed = sch.symbol.value_matches(r'^PWR_FLAG$')

    if not (vcc_seed and gnd_seed and flg_seed):
        raise RuntimeError("Seed schematic must contain power symbols with values VCC, GND, and PWR_FLAG.")

    cx, cy = xy(chip)

    vcc = vcc_seed[0].clone()
    gnd = gnd_seed[0].clone()
    flg = flg_seed[0].clone()

    vcc.move(cx - 10, POWER_ROW_Y)
    flg.move(cx + 0,  POWER_ROW_Y)
    gnd.move(cx + 10, POWER_ROW_Y + 5)

    # labels and stubs
    l_v = sch.label.new(); l_v.value = "VCC"; l_v.move(vcc.at.value[0], vcc.at.value[1] + 5)
    w_v = sch.wire.new();  w_v.start_at(l_v); w_v.end.value = [l_v.at.value[0], l_v.at.value[1] + 3]

    l_g = sch.label.new(); l_g.value = "GND"; l_g.move(gnd.at.value[0], gnd.at.value[1] - 5)
    w_g = sch.wire.new();  w_g.start_at(l_g); w_g.end.value = [l_g.at.value[0], l_g.at.value[1] + 3]

def make_header(sch, seed_header, at_xy, ref_base="J"):
    """
    Clone the 2x20 header template to a new position with a fresh reference.
    """
    h = seed_header.clone()
    # Choose next ref (J2, J3, …) avoiding collisions
    existing = [s.property.Reference.value for s in sch.symbol]
    h.property.Reference.value = next_ref(ref_base, existing)
    h.move(*at_xy)
    return h

def label_row_for_bus(sch, bus_names, anchor_xy):
    """
    Place a vertical stack of global labels (A[7], A[6], ...) near a header.
    """
    x, y = anchor_xy
    gap = 2.54  # 100 mils
    out_labels = []
    for i, n in enumerate(bus_names):
        gl = sch.global_label.new()
        gl.value = n
        gl.move(x, y + i*gap)
        out_labels.append(gl)
    return out_labels

# ---------- main build ----------
def main():
    # 1) Load yosys ports
    ports = load_ports_from_yosys(DESIGN_JSON)

    # 2) Open seed, grab templates
    sch = skip.Schematic(SEED_SCH)

    # Sanity check: seed contains required symbols
    strict_refs = ["U1", "J1", "C1"]
    for r in strict_refs:
        if len(sch.symbol.reference_matches(fr'^{re.escape(r)}$')) == 0:
            raise RuntimeError(f"Seed schematic missing required symbol {r}")

    if not sch.symbol.value_matches(r'^VCC$'):
        raise RuntimeError("Seed schematic missing VCC power symbol (value=VCC).")
    if not sch.symbol.value_matches(r'^GND$'):
        raise RuntimeError("Seed schematic missing GND power symbol (value=GND).")
    if not sch.symbol.value_matches(r'^PWR_FLAG$'):
        raise RuntimeError("Seed schematic missing PWR_FLAG power symbol (value=PWR_FLAG).")

    chip = sch.symbol.U1
    seed_header = sch.symbol.J1

    # 3) Move chip to where we want it and ensure its lib_id matches the generated symbol
    chip.move(*CHIP_POS)
    # If your seed’s U1 already points to MatMulChip, this is optional.
    if hasattr(chip, "lib_id"):
        chip.lib_id.value = CHIP_LIBID
    # Update visible value to something descriptive
    chip.property.Value.value = "MatMulChip (auto)"

    # 4) Add power cluster + decouplers
    add_power_bundle(sch, chip)
    place_decouplers(sch, chip, count=4, value="0.1uF")

    # 5) Place three headers: A, B, OUT
    hA = make_header(sch, seed_header, HEAD_A_POS, ref_base="J")
    hB = make_header(sch, seed_header, HEAD_B_POS, ref_base="J")
    hO = make_header(sch, seed_header, HEAD_OUT_POS, ref_base="J")

    # 6) Label stacks near each header for bus breakouts
    # Decide which ports belong to which bus group by prefix
    buses = {}
    for name, width, direction in ports:
        key = None
        for pref in BUS_TO_HEADER:
            if name.startswith(pref):
                key = pref
                break
        if key is None:
            # Put scalars or misc control (CLK,RST,EN, etc.) near the chip
            if width == 1:
                gl = sch.global_label.new(); gl.value = name
                gl.move(chip.at.x - 20, chip.at.y + 20)  # park it on the left
                # (optional) draw a tiny stub so it’s clearly “on a net”
                w = sch.wire.new(); w.start_at(gl); w.end.value = [gl.at.x+2, gl.at.y]
            continue
        buses.setdefault(key, []).append((name, width))

    # Build label rows
    # Left: A bus, Right: B bus, Bottom: OUT bus
    if "A" in buses:
        names = []
        for n,w in buses["A"]:
            names.extend(expand_bus(n,w))
        hA_x, hA_y = xy(hA)
        label_row_for_bus(sch, names, (hA_x - 6, hA_y - 25))
    if "B" in buses:
        names = []
        for n,w in buses["B"]:
            names.extend(expand_bus(n,w))
        hB_x, hB_y = xy(hB)
        label_row_for_bus(sch, names, (hB_x + 6, hB_y - 25))
    if "OUT" in buses:
        names = []
        for n,w in buses["OUT"]:
            names.extend(expand_bus(n,w))
        hO_x, hO_y = xy(hO)
        label_row_for_bus(sch, names, (hO_x - 25, hO_y + 6))

    # (Optional) add CLK/RST labels if present, near bottom header too
    for name, w, d in ports:
        if name in ("CLK","RST","RESET","EN") and w == 1:
            gl = sch.global_label.new(); gl.value = name
            hO_x, hO_y = xy(hO)
            gl.move(hO_x + 25, hO_y + 6)

    # 7) Save schematic
    # sch.write(OUT_SCH)
    # print(f"Wrote {OUT_SCH}")
    tmp_out = OUT_SCH + ".tmp"
    sch.write(tmp_out)
    sanitize_header_and_title(tmp_out, OUT_SCH, title="MatMulChip Auto")
    os.remove(OUT_SCH + ".tmp")
    print(f"Wrote {OUT_SCH}")

if __name__ == "__main__":
    main()
