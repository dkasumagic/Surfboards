#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <cstdint>
#include "verilog_writer.hpp"

int writeVerilogFile(const std::string& inputFileName, const std::string& outputFileName, const size_t& numBits) {
    // HUMAN COMMENTS btw haha
    // Initalise my files
    std::ifstream inputFile { inputFileName };
    std::ofstream outputFile { outputFileName };
    std::string line;
   
    if (inputFile.is_open() && outputFile.is_open()) {
        // Initalise some more stuff and get the matrix size
        uint64_t row, col;
        std::getline(inputFile, line);
        std::istringstream iss(line);
        iss >> row >> col;
        const uint64_t maxCount = row * col;
        const uint64_t maxIndex = (row * col)-1;
        const bool signBit = true; // TODO: eventually make this a user option.
 
        // Write the preamble
        outputFile 
            << "`timescale 1ns/1ps\n\n"
            << "module surfboard #(\n"
            << "\tparameter int W = " << numBits << ",\n"
            << "\tparameter bit SIGNED = " << signBit
            << "\n)(\n"
            << "\tinput  logic [0:" << maxIndex << "][W-1:0] A,\n"
            << "\tinput  logic [0:" << maxIndex << "][W-1:0] B,\n"
            << "\toutput logic [0:" << maxIndex << "][W-1:0] C\n"
            << ");\n"
            << "\tlogic signed [0:" << maxIndex << "][W-1:0] As;\n"
            << "\tlogic signed [0:" << maxIndex << "][W-1:0] Bs;\n"
            << "\tlogic        [0:" << maxIndex << "][W-1:0] Au;\n"
            << "\tlogic        [0:" << maxIndex << "][W-1:0] Bu;\n\n"
            << "\tassign Au = A; assign Bu = B;\n"
            << "\tassign As = A; assign Bs = B;\n\n"
            << "\tfunction automatic logic [W-1:0] mul(input int i, input int j);\n"
            << "\t\tif (SIGNED) mul = As[i] * Bs[j];\n"
            << "\t\telse mul = Au[i] * Bu[j];\n"
            << "\tendfunction\n\n";

        // Actual Meat and Potatoes
        uint64_t a { 0 }, b { 0 }, c { 0 };
        char op;
        for (; c < maxCount; c++) {
            bool first = true; // TODO: is this the best way to do this?
            iss.clear();
            std::getline(inputFile, line);
            iss.str(line);
            
            outputFile << "\tassign C[" << c << "] = ";
            while (iss >> a >> op >> b) {
                if (first) first = false;
                else outputFile << " + ";
                outputFile << "mul(" << a << "," << b << ")";
            }
            outputFile << ";\n";
        }
        outputFile << "\n" << "endmodule\n";

        outputFile.close();
        std::cout << "Finish.\n";
    } else {
        std::cerr << "Error: could not open file/s: "
            << (!inputFile.is_open() ? inputFileName : "")
            << (!outputFile.is_open() ? outputFileName : "")
            << std::endl;
    }

    return 0;
}

// int main() {
//     writeVerilogFile("./build/MH_output_22.txt", "./build/verilog.sv", 4);
//     return 0;
// }