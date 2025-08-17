#ifndef SVD_VERILOG_WRITER_HPP
#define SVD_VERILOG_WRITER_HPP

#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <cstdint>

int svd_verilog_writer(std::string& inputFile, std::string& outputFile, int numBits, 
                       int n, int m, int r, int s1, int s2);

#endif