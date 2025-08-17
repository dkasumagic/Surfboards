import json, csv, sys

design = json.load(open(sys.argv[1]))
top = next((m for m in design["modules"] if "ports" in design["modules"][m]), None)
ports = design["modules"][top]["ports"]

rows = []
pin_num = 1

def expand(name, bits):
  idxs = list(range(len(bits)-1, -1, -1)) if len(bits)>1 else [None]
  for i in idxs:
    yield (f"{name}[{i}]" if i is not None else name)

power_plan = ["VCC", "GND"] * 4

for k in sorted(ports):
  p = ports[k]
  direction = p["direction"]  # input/output/inout
  names = list(expand(k, p["bits"]))
  side = "left" if direction=="input" else ("right" if direction=="output" else "top")
  ptype = {"input":"input","output":"output"}.get(direction, "bidirectional")
  for n in names:
    rows.append([pin_num, n, ptype, "", side])
    pin_num += 1

for n in power_plan:
  rows.append([pin_num, n, "power_in", "", "bottom"])
  pin_num += 1

with open("build/pins.csv","w",newline="") as f:
  w = csv.writer(f)
  w.writerow(["MatMulChip", "U", "Package_QFP:LQFP-100_14x14mm_P0.5mm"])
  w.writerow(["Pin","Name","Type","Unit","Side"])
  w.writerows(rows)
print("Wrote build/pins.csv")

# CLI WORKFLOW FOR 3D MODEL OUTPUT
# Call from project_dir: python3 ./src/backend/ports_to_csv.py ./build/design.json
# Then generate a legacy kicad 6 symbol library with: kipart build/pins.csv -w -o build/MatMulChip.lib -s row -b
# Then use kicad-cli to upgrade it to a kicad 9 symbol library: kicad-cli sym upgrade build/MatMulChip.lib -o build/MatMulChip.kicad_sym
# Then run it through the construct skip schematic script: python3 ./src/backend/skip_schematic.py
# Then annotate the references: python3 ./src/backend/annotate.py ./build/auto.kicad_sch
# Then open it with kicad too press the "update pcb with schemiatic button"
# Then run these

# kicad-cli pcb export gerbers build/auto/auto.kicad_pcb -o build/hardware/gerbers
# kicad-cli pcb export drill   build/auto/auto.kicad_pcb -o build/hardware/gerbers


# kicad-cli pcb export step    build/auto/auto.kicad_pcb -o build/hardware/board.step

# kicad-cli pcb export vrml    build/auto/auto.kicad_pcb -o build/hardware/board.wrl
# kicad-cli pcb export glb     build/auto/auto.kicad_pcb -o build/hardware/board.glb
