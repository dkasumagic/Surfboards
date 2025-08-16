// TODO: implement kicad generation -> generate 3d model of the PCB
#include <iostream>
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

SynthesisResult runSynthesis(const std::filesystem::path& verilogFilePath, const std::filesystem::path& yosysFilePath, bool quiteYosys) {
  SynthesisResult result {
    .code { -1 },
    .message { "Undefined Error" },
    .design_json { "build/design.json" },
    .schematic_svg { "build/schematic.svg" },
    .yosys_log { "build/yosys.log" },
    .netlistsvg_log { "build/netlistsvg.log" }
  };

  std::cout << "Running Yosys" << std::endl;
  {
    std::vector<std::string> command;
    command.push_back("yosys");  // TODO: add yosys to requirements.
    if (quiteYosys) command.push_back("-q");
    command.push_back("-s");
    command.push_back(yosysFilePath.string());

    int code = run_command(command, result.yosys_log);

    if (code) {
      result.code = 1;
      result.message = "Yosys failed (exit " + std::to_string(code) + "). See log: " + result.yosys_log.string();
      return result;
    }
    if (!file_exists(result.design_json)) {
      result.code = 2;
      result.message = "Yosys completed but design.json not found at: " + result.design_json.string();
      return result;
    }
  }

  std::cout << "Running Netlistsvg" << std::endl;
  {
    std::vector<std::string> command;
    command.push_back("npx");
    command.push_back("netlistsvg");
    command.push_back(result.design_json.string());
    command.push_back("-o");
    command.push_back(result.schematic_svg.string());

    int code = run_command(command, result.netlistsvg_log);

    if (code) {
      result.code = 1;
      result.message = "Netlistsvg failed (exit " + std::to_string(code) + "). See log: " + result.netlistsvg_log.string();
      return result;
    }
    if (!file_exists(result.netlistsvg_log)) {
      result.code = 2;
      result.message = "Netlistsvg completes but schematic.svg not found at: " + result.schematic_svg.string();
      return result;
    }
  }

  result.code = 0;
  result.message = "Synthesis Success";
  return result;
}

// int main() {
//   SynthesisResult result = run_synthesis("build/verilog_out.sv", "src/backend/synth.ys");
//   std::cout
//     << result.code << std::endl
//     << result.message << std::endl
//     << result.design_json << std::endl
//     << result.schematic_svg << std::endl
//     << result.yosys_log << std::endl
//     << result.netlistsvg_log << std::endl;

//     return 0;
// }
