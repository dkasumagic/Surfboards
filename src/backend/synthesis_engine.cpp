// takes the verilog file and generates an altium PCB specification

// 1. use yosys script to generatge build/design.json
// yosys -s ./src/backend/synth.ys

// 2. use netlistsvg to create the schematic 
// npx netlistsvg ./build/design.json -o ./build/schematic.svg

// Extension: 3. make the schematic work with kicad. generate a 3d model of the PCB

#include <fstream>
#include "synthesis_engine.hpp"

static bool file_exists(const std::filesystem::path& path) {
  std::error_code code;
  return std::filesystem::exists(path, code)
    && std::filesystem::is_regular_file(path, code);
}

static int run_command(const std::vector<std::string>& cmd, const std::filesystem::path& logfilePath) {
  std::ofstream touch(logfilePath.string(), std::ios::out | std::ios::trunc);
  touch.close();

  std::string join;
  for (size_t i = 0; i < cmd.size(); i++) {
    if (i) join += " ";
    join += cmd[i];
  }
  join += " > \"" + logfilePath.string() + "\" 2>&1";

  int run = std::system(join.c_str());
  if (run == -1) return -9998;
  return WEXITSTATUS(run);
}

