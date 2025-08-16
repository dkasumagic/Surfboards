// takes the verilog file and generates an altium PCB specification

// 1. use yosys script to generatge build/design.json
// yosys -s ./src/backend/synth.ys

// 2. use netlistsvg to create the schematic 
// npx netlistsvg ./build/design.json -o ./build/schematic.svg