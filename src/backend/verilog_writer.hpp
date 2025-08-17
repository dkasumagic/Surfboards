#ifndef VERILOG_WRITER_HPP
#define VERILOG_WRITER_HPP

#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <cstdint>

int writeVerilogFile(const std::string& inputFileName, const std::string& outputFileName, const size_t& numBits);

#endif
