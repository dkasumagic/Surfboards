#ifndef VERILOG_WRITER.HPP
#define VERILOG_WRITER.HPP

// Takes the matrixhandler output and produces valid verilog code.
#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <cstdint>

int write_verilog_file(const std::string& inputFileName, const std::string& outputFileName);

#endif
