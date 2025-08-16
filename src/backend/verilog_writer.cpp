// Takes the matrixhandler output and produces valid verilog code.
#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <cstdint>

int write_verilog_file(const std::string& inputFileName, const std::string& outputFileName, int numBits) {
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
        // const uint64_t numBits = n; // TODO: eventually make this a user option
        const bool signBit = true; // TODO: eventually make this a user option.
 
        // Write the preamble
        outputFile 
            << "`timescale 1ns/1ps\n\n"
            << "module surfboard #(\n"
            << "\tparameter int W = " << numBits << ",\n"// W is number of input bits
            << "\tparameter bit SIGNED = " << signBit << "\n" // is it 2's compliment.
            << ")(\n"
            << "\tinput  logic [W-1:0] A [0:" << maxIndex << "],\n"
            << "\tinput  logic [W-1:0] B [0:" << maxIndex << "],\n"
            << "\toutput logic [SUMW-1:0] C [0:" << maxIndex << "]\n"
            << ");\n"
            << "\tlocalparam int PROD_W = 2*W;\n" // This creates 32bit wire 
            << "\tlocalparam int SUMW   = PROD_W + 1;\n\n" //
            << "\tlogic signed   [W-1:0] As [0:" << maxIndex << "];\n" // Don't think we need signed here.
            << "\tlogic signed   [W-1:0] Bs [0:" << maxIndex << "];\n"
            << "\tlogic          [W-1:0] Au [0:" << maxIndex << "];\n"
            << "\tlogic          [W-1:0] Bu [0:" << maxIndex << "];\n\n"
            << "\tassign Au = A; assign Bu = B;\n"
            << "\tassign As = A; assign Bs = B;\n\n"
            << "\tfunction automatic logic [W-1:0] mul(input int i, input int j);\n"
            << "\t\tlogic [PROD_W-1:0] tempResult;\n"
            << "\t\tif (SIGNED)\n"
            << "\t\t\ttempResult = As[i] * Bs[j];\n" // '*' needs to be changed out for a module
            << "\t\telse\n"
            << "\t\t\ttempResult = Au[i] * Bu[j];\n"
            << "\t\tmul = tempResult[3*(2*PROD_W)/4 - 2: PROD_W/4 - 1];\n"
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
            << (!inputFile.is_open() ? inputFileName + ".txt " : "")
            << (!outputFile.is_open() ? outputFileName + ".txt " : "")
            << std::endl;
    }

    return 0;
}
