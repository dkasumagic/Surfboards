#!/usr/bin/env python3

# IDK IF WE'LL USE THIS

import sys, re, argparse
from pathlib import Path
import skip  # pip install kicad-skip

REF_RE = re.compile(r"^([A-Za-z]+)(\?|\d+)([A-Za-z_]*)$")

def ref_prefix(ref: str):
    """Return the alphabetic prefix for a ref (e.g., 'J' from 'J?'/'J3')."""
    m = REF_RE.match(ref)
    return m.group(1) if m else None

def ref_number(ref: str):
    """Return the numeric part as int if present, else None."""
    m = REF_RE.match(ref)
    if not m: return None
    try:
        return int(m.group(2))
    except ValueError:
        return None  # when it's '?'

def is_powerish(ref: str):
    """Skip power flags/symbols etc that start with '#', e.g. #PWR0123, #FLG01."""
    return ref.startswith("#")

def xy(sym):
    """Get (x, y) from a skip symbol's .at.value."""
    v = getattr(sym, "at", None)
    if not v or v.value is None or len(v.value) < 2:
        return (0.0, 0.0)
    return float(v.value[0]), float(v.value[1])

def annotate(sch_path: Path, dry_run: bool = False, verbose: bool = True):
    schem = skip.Schematic(str(sch_path))

    # Collect by prefix
    by_prefix = {}          # prefix -> {'existing': set(int), 'todo': [sym,...]}
    all_syms = [s for s in schem.symbol]

    for s in all_syms:
        # Every symbol should have a Reference property
        try:
            ref = s.property.Reference.value
        except Exception:
            continue

        if not ref or is_powerish(ref):
            continue

        pref = ref_prefix(ref)
        if not pref:
            continue

        entry = by_prefix.setdefault(pref, {"existing": set(), "todo": []})
        num = ref_number(ref)

        if num is None:  # 'J?' etc
            entry["todo"].append(s)
        else:
            entry["existing"].add(num)

    # For deterministic numbering, sort each TODO by position (top-to-bottom, then left-to-right)
    for pref, group in by_prefix.items():
        group["todo"].sort(key=lambda s: (xy(s)[1], xy(s)[0]))

    # Assign numbers
    changes = []
    for pref, group in sorted(by_prefix.items()):
        used = set(group["existing"])
        # start from max(existing)+1 or 1 if none
        start = max(used) + 1 if used else 1
        next_num = start
        for s in group["todo"]:
            # find first free number
            while next_num in used:
                next_num += 1
            old = s.property.Reference.value
            new = f"{pref}{next_num}"
            if verbose:
                changes.append((old, new))
            if not dry_run:
                s.property.Reference.value = new
            used.add(next_num)
            next_num += 1

    if verbose and changes:
        print("Planned changes:")
        for old, new in changes:
            print(f"  {old} -> {new}")
    elif verbose:
        print("Nothing to annotate.")

    if not dry_run:
        # backup then write in-place
        backup = sch_path.with_suffix(sch_path.suffix + ".bak")
        backup.write_text(Path(sch_path).read_text(encoding="utf-8"), encoding="utf-8")
        schem.write(str(sch_path))
        if verbose:
            print(f"Annotated and saved. Backup: {backup}")

def main():
    ap = argparse.ArgumentParser(description="Annotate KiCad schematic refs using skip.")
    ap.add_argument("schematic", help="Path to .kicad_sch")
    ap.add_argument("--dry-run", action="store_true", help="Show changes, do not write")
    ap.add_argument("-q", "--quiet", action="store_true", help="No change listing")
    args = ap.parse_args()

    path = Path(args.schematic)
    if not path.exists():
        print(f"Error: {path} not found")
        sys.exit(1)

    annotate(path, dry_run=args.dry_run, verbose=not args.quiet)

if __name__ == "__main__":
    main()
