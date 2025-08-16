#ifndef SYNTHESIS_ENGINE_HPP
#define SYNTHESIS_ENGINE_HPP

#include <string>
#include <vector>
// #include <optional>
// #include <iostream>
// #include <fstream>
#include <filesystem>

struct synthesisResult {
  bool ok;
  std::string message;
  std::filesystem::path design_json;
  std::filesystem::path schematic_svg;
  std::filesystem::path yosys_log;
  std::filesystem::path netlistsvg_log;
};

static bool file_exists(const std::filesystem::path& path);
static int run_command(const std::vector<std::string>& cmd, const std::filesystem::path& logfilePath);

#endif