
#!/usr/bin/env python3
'''
yosys_json_to_kicad.py

Convert a Yosys JSON netlist (write_json) into a KiCad schematic (.kicad_sch).
Also emits a tiny symbol library with the gates we map to and a project sym-lib-table.

Supported cell types (from a small Liberty / abc mapping):
  - INV (A->Y)
  - BUF (A->Y)
  - NAND2, NOR2, XOR2, XNOR2 (A,B->Y)
  - MUX2 (A,B,S->Y)
  - DFF (D,C->Q)    # clock pin named 'C' to match many basic libs
          
          
In terminal use this:
python3 ./src/backend/to_kicad_sch.py --json build/design.json --out kicad_out --top surfboard
          
Connectivity approach:
  For each cell pin, we draw a short wire from the pin to a nearby point,
  and place a local net label at that point. KiCad connects all items using
  the same local label across the sheet; no need to route wires between gates.

Usage:
  python3 yosys_json_to_kicad.py --json build/design.json --out kicad_out --top TOPMODULE

This script will create:
  - kicad_out/HackathonLib.kicad_sym      (symbol library with basic gates)
  - kicad_out/sym-lib-table               (project sym-lib-table referencing the library)
  - kicad_out/design.kicad_sch            (schematic file referencing symbols in the lib)
'''
import argparse
import json
from pathlib import Path
from uuid import uuid4

# --------------------------- KiCad helpers ---------------------------

def _uuid():
    return str(uuid4())

def ksym_header():
    return '(kicad_symbol_lib (version 20211014) (generator "yosys-json-to-kicad"))\n'

def ksym_symbol_block(name, pins, body_rect=(-4.0, -4.0, 4.0, 4.0)):
    '''
    Create a single symbol definition:
      name: "HackathonLib:INV" etc. (library_id)
      pins: list of dicts: [{"number":"1","name":"A","type":"input","at":(-5,0)}, ...]
      body_rect: rectangle (x1,y1,x2,y2)
    '''
    x1, y1, x2, y2 = body_rect
    lines = []
    lines.append(f'  (symbol "{name}" (pin_names (offset 0)) (in_bom yes) (on_board yes)')
    lines.append(f'    (property "Reference" "U" (at 0 -6 0) (effects (font (size 1.27 1.27))))')
    lines.append(f'    (property "Value" "{name.split(":")[-1]}" (at 0 6 0) (effects (font (size 1.27 1.27))))')
    lines.append(f'    (symbol "body"')
    lines.append(f'      (rectangle (start {x1} {y1}) (end {x2} {y2}) (stroke (width 0.15) (type solid)) (fill (type background)))')
    for p in pins:
        pnum = p["number"]
        pname = p["name"]
        ptype = p.get("type","input")
        px, py = p["at"]
        length = p.get("length", 1.8)
        # KiCad pin syntax: (pin <type> line (at X Y) (length L) (name "X") (number "N"))
        lines.append(f'      (pin {ptype} line (at {px} {py}) (length {length})'
                     f' (name "{pname}" (effects (font (size 1.27 1.27)))) (number "{pnum}"))')
    lines.append(f'    )')
    lines.append(f'  )')
    return "\n".join(lines) + "\n"

def write_symbol_library(lib_path: Path):
    '''
    Emit HackathonLib.kicad_sym with INV, BUF, NAND2, NOR2, XOR2, XNOR2, MUX2, DFF
    '''
    lib_name = "HackathonLib"
    content = [ksym_header()]
    # INV: A->Y
    content.append(ksym_symbol_block(f"{lib_name}:INV", [
        {"number":"1","name":"A","type":"input","at":(-5,0),"length":1.8},
        {"number":"2","name":"Y","type":"output","at":(5,0),"length":1.8},
    ]))
    # BUF: A->Y
    content.append(ksym_symbol_block(f"{lib_name}:BUF", [
        {"number":"1","name":"A","type":"input","at":(-5,0),"length":1.8},
        {"number":"2","name":"Y","type":"output","at":(5,0),"length":1.8},
    ]))
    # 2-input gates: A,B -> Y
    for gate in ["NAND2","NOR2","XOR2","XNOR2"]:
        content.append(ksym_symbol_block(f"{lib_name}:{gate}", [
            {"number":"1","name":"A","type":"input","at":(-5,-2),"length":1.8},
            {"number":"2","name":"B","type":"input","at":(-5, 2),"length":1.8},
            {"number":"3","name":"Y","type":"output","at":(5,0),"length":1.8},
        ]))
    # MUX2: A,B,S -> Y
    content.append(ksym_symbol_block(f"{lib_name}:MUX2", [
        {"number":"1","name":"A","type":"input","at":(-5,-3),"length":1.8},
        {"number":"2","name":"B","type":"input","at":(-5, 0),"length":1.8},
        {"number":"3","name":"S","type":"input","at":(-5, 3),"length":1.8},
        {"number":"4","name":"Y","type":"output","at":(5,0),"length":1.8},
    ]))
    # DFF: D,C -> Q
    content.append(ksym_symbol_block(f"{lib_name}:DFF", [
        {"number":"1","name":"D","type":"input","at":(-5, 0),"length":1.8},
        {"number":"2","name":"C","type":"input","at":(-5, 3),"length":1.8},
        {"number":"3","name":"Q","type":"output","at":(5, 0),"length":1.8},
    ], body_rect=(-5.0,-5.0,5.0,5.0)))
    lib_path.write_text("".join(content), encoding="utf-8")

def write_sym_lib_table(table_path: Path, lib_rel_uri: str = "${KIPRJMOD}/HackathonLib.kicad_sym"):
    content = (
        "(sym_lib_table\n"
        f'  (lib (name "HackathonLib")(type "KiCad")(uri "{lib_rel_uri}")(options "")(descr ""))\n'
        ")\n"
    )
    table_path.write_text(content, encoding="utf-8")

# --------------------------- Yosys JSON parsing ---------------------------

def pick_top_module(modules: dict, explicit: str | None):
    if explicit and explicit in modules:
        return explicit
    for name, m in modules.items():
        if m.get("attributes", {}).get("top", "0") in ("1", 1, True):
            return name
    if len(modules) == 1:
        return next(iter(modules.keys()))
    return next(iter(modules.keys()))

def build_bit_to_name(module: dict) -> dict:
    '''
    Map bit index (int) or const string ('0','1') -> net name.
    For buses, yield name[i] per bit.
    '''
    mapping = {}
    netnames = module.get("netnames", {})
    for nname, nd in netnames.items():
        bits = nd.get("bits", [])
        if len(bits) == 1:
            b = bits[0]
            mapping[b] = nname
        else:
            for i, b in enumerate(bits):
                mapping[b] = f"{nname}[{i}]"
    return mapping

def bit_to_str(bit, bitname_map: dict) -> str:
    if isinstance(bit, str):
        if bit == '0': return 'CONST_0'
        if bit == '1': return 'CONST_1'
        return f'CONST_{bit.upper()}'
    return bitname_map.get(bit, f"NET_{bit}")

# --------------------------- Schematic generation ---------------------------

def ksch_header():
    return '(kicad_sch (version 20211014) (generator "yosys-json-to-kicad")\n  (paper "A4")\n'

def ksch_footer():
    return ')\n'

def ksch_symbol(lib_id, ref, atx, aty, value=None):
    if value is None:
        value = lib_id.split(":")[-1]
    u = _uuid()
    s = []
    s.append(f'  (symbol (lib_id "{lib_id}") (at {atx} {aty} 0) (unit 1) (in_bom yes) (on_board yes)')
    s.append(f'    (property "Reference" "{ref}" (at 0 -6 0) (effects (font (size 1.27 1.27))))')
    s.append(f'    (property "Value" "{value}" (at 0 6 0) (effects (font (size 1.27 1.27))))')
    s.append(f'    (property "Footprint" "" (at 0 0 0) (effects (font (size 1.27 1.27))) (hide yes))')
    s.append(f'    (property "Datasheet" "" (at 0 0 0) (effects (font (size 1.27 1.27))) (hide yes))')
    s.append(f'    (uuid {u})')
    s.append(f'  )')
    return "\n".join(s)

def ksch_wire(x1, y1, x2, y2):
    u = _uuid()
    return f'  (wire (pts (xy {x1} {y1}) (xy {x2} {y2})) (uuid {u}))'

def ksch_label(name, x, y):
    u = _uuid()
    return f'  (label "{name}" (at {x} {y} 0) (effects (font (size 1.27 1.27))) (uuid {u}))'

PIN_OFFSETS = {
    "HackathonLib:INV":   {"A": (-5,  0), "Y": ( 5,  0)},
    "HackathonLib:BUF":   {"A": (-5,  0), "Y": ( 5,  0)},
    "HackathonLib:NAND2": {"A": (-5, -2), "B": (-5, 2), "Y": (5, 0)},
    "HackathonLib:NOR2":  {"A": (-5, -2), "B": (-5, 2), "Y": (5, 0)},
    "HackathonLib:XOR2":  {"A": (-5, -2), "B": (-5, 2), "Y": (5, 0)},
    "HackathonLib:XNOR2": {"A": (-5, -2), "B": (-5, 2), "Y": (5, 0)},
    "HackathonLib:MUX2":  {"A": (-5, -3), "B": (-5, 0), "S": (-5, 3), "Y": (5, 0)},
    "HackathonLib:DFF":   {"D": (-5, 0), "C": (-5, 3), "Q": (5, 0)},
}

CELL_MAP = {
    "INV":   ("HackathonLib:INV",   {"A":"A","Y":"Y"}),
    "BUF":   ("HackathonLib:BUF",   {"A":"A","Y":"Y"}),
    "NAND2": ("HackathonLib:NAND2", {"A":"A","B":"B","Y":"Y"}),
    "NOR2":  ("HackathonLib:NOR2",  {"A":"A","B":"B","Y":"Y"}),
    "XOR2":  ("HackathonLib:XOR2",  {"A":"A","B":"B","Y":"Y"}),
    "XNOR2": ("HackathonLib:XNOR2", {"A":"A","B":"B","Y":"Y"}),
    "MUX2":  ("HackathonLib:MUX2",  {"A":"A","B":"B","S":"S","Y":"Y"}),
    "DFF":   ("HackathonLib:DFF",   {"D":"D","C":"C","Q":"Q"}),
}

def place_cells_grid(cells):
    coords = {}
    per_col = 12
    xstep = 20
    ystep = 12
    col = 0
    row = 0
    for cname in cells:
        x = col * xstep
        y = row * ystep
        coords[cname] = (x, y)
        row += 1
        if row >= per_col:
            row = 0
            col += 1
    return coords

def gen_schematic(modname, module, out_sch_path: Path):
    bitmap = build_bit_to_name(module)
    cells = module.get("cells", {})
    cell_names = list(cells.keys())
    placements = place_cells_grid(cell_names)

    chunks = [ksch_header()]

    ref_counter = 1
    for cname in cell_names:
        cell = cells[cname]
        ctype = cell.get("type", "")
        if ctype not in CELL_MAP:
            print(f"[WARN] Skipping unsupported cell type: {ctype} ({cname})")
            continue
        lib_id, portmap = CELL_MAP[ctype]
        atx, aty = placements[cname]
        ref = f"U{ref_counter}"
        ref_counter += 1
        chunks.append(ksch_symbol(lib_id, ref, atx, aty, value=ctype))

        conns = cell.get("connections", {})
        pin_offsets = PIN_OFFSETS[lib_id]
        for port, bits in conns.items():
            if port not in portmap:
                continue
            if len(bits) != 1:
                iter_bits = list(enumerate(bits))
            else:
                iter_bits = [(None, bits[0])]
            for bi, b in iter_bits:
                netname = bit_to_str(b, bitmap)
                sym_pin = portmap[port]
                if sym_pin not in pin_offsets:
                    continue
                dx, dy = pin_offsets[sym_pin]
                pin_x = atx + dx
                pin_y = aty + dy
                if dx < 0:
                    lx = pin_x - 3.0
                    wx1, wy1, wx2, wy2 = pin_x, pin_y, lx, pin_y
                else:
                    lx = pin_x + 3.0
                    wx1, wy1, wx2, wy2 = pin_x, pin_y, lx, pin_y
                final_name = f"{netname}" if bi is None else f"{netname}[{bi}]"
                chunks.append(ksch_wire(wx1, wy1, wx2, wy2))
                chunks.append(ksch_label(final_name, lx, pin_y))

    chunks.append(ksch_footer())
    out_sch_path.write_text("\n".join(chunks), encoding="utf-8")

def pick_top_module(modules: dict, explicit: str | None):
    if explicit and explicit in modules:
        return explicit
    for name, m in modules.items():
        if m.get("attributes", {}).get("top", "0") in ("1", 1, True):
            return name
    if len(modules) == 1:
        return next(iter(modules.keys()))
    return next(iter(modules.keys()))

def build_bit_to_name(module: dict) -> dict:
    mapping = {}
    netnames = module.get("netnames", {})
    for nname, nd in netnames.items():
        bits = nd.get("bits", [])
        if len(bits) == 1:
            b = bits[0]
            mapping[b] = nname
        else:
            for i, b in enumerate(bits):
                mapping[b] = f"{nname}[{i}]"
    return mapping

def bit_to_str(bit, bitname_map: dict) -> str:
    if isinstance(bit, str):
        if bit == '0': return 'CONST_0'
        if bit == '1': return 'CONST_1'
        return f'CONST_{bit.upper()}'
    return bitname_map.get(bit, f"NET_{bit}")

def main():
    import argparse
    ap = argparse.ArgumentParser()
    ap.add_argument("--json", required=True, help="Path to Yosys write_json output (e.g., build/design.json)")
    ap.add_argument("--out", default="kicad_out", help="Output directory for KiCad files")
    ap.add_argument("--top", default=None, help="Top module name (optional; auto-detected otherwise)")
    args = ap.parse_args()

    jpath = Path(args.json)
    outdir = Path(args.out)
    outdir.mkdir(parents=True, exist_ok=True)

    data = json.loads(jpath.read_text(encoding="utf-8"))
    modules = data.get("modules", {})
    if not modules:
        raise SystemExit("No modules in JSON. Did you pass the right file?")

    top = pick_top_module(modules, args.top)
    if top not in modules:
        raise SystemExit(f"Top module '{top}' not found in JSON modules: {list(modules.keys())}")
    module = modules[top]

    write_symbol_library(outdir / "HackathonLib.kicad_sym")
    write_sym_lib_table(outdir / "sym-lib-table", lib_rel_uri='${{KIPRJMOD}}/HackathonLib.kicad_sym')

    gen_schematic(top, module, outdir / "design.kicad_sch")

    print("[OK] Wrote:")
    print(f"  - {outdir/'HackathonLib.kicad_sym'}")
    print(f"  - {outdir/'sym-lib-table'}")
    print(f"  - {outdir/'design.kicad_sch'}")
    print("Open design.kicad_sch in KiCad. If KiCad can’t find the library, verify the sym-lib-table is in the same folder as the schematic, or add the library via Preferences → Manage Symbol Libraries.")

if __name__ == "__main__":
    main()
