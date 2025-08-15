// Takes the matrixhandler output and produces valid verilog code.
#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <cstdint>

int write_verilog_file(const std::string& inputFileName, const std::string& outputFileName) {
    // HUMAN COMMENTS btw haha
    // Initalise my stuff
    std::ifstream inputFile { inputFileName };
    std::ofstream outputFile { outputFileName };
    std::string line;
   
    if (inputFile.is_open() && outputFile.is_open()) {
        // Initalise some stuff and get the matrix size
        uint64_t row, col;
        std::getline(inputFile, line);
        std::istringstream iss(line);
        iss >> row >> col;
        const uint64_t maxCount = row * col;
        const uint64_t maxIndex = (row * col)-1;
        const uint64_t numBits = 16;
        const bool signBit = true;

        // Write the preamble
        outputFile 
            << "`timescale 1ns/1ps" << std::endl << std::endl
            << "module surfboard #(" << std::endl
            << "\tparameter int W = " << numBits << "," << std::endl // TODO: eventually make this a user option
            << "\tparameter bit SIGNED = " << signBit << std::endl // TODO: eventually make this a user option
            << ")(" << std::endl
            << "\tinput  logic [W-1:0] A [0:" << maxIndex << "]," << std::endl
            << "\tinput  logic [W-1:0] B [0:" << maxIndex << "]," << std::endl
            << "\toutput logic [SUMW-1:0] C [0:" << maxIndex << "]" << std::endl
            << ");" << std::endl
            << "\tlocalparam int PROD_W = 2*W;" << std::endl
            << "\tlocalparam int SUMW   = PROD_W + 1;" << std::endl << std::endl
            << "\tlogic signed   [W-1:0] As [0:" << maxIndex << "];" << std::endl
            << "\tlogic signed   [W-1:0] Bs [0:" << maxIndex << "];" << std::endl
            << "\tlogic          [W-1:0] Au [0:" << maxIndex << "];" << std::endl
            << "\tlogic          [W-1:0] Bu [0:" << maxIndex << "];" << std::endl << std::endl
            << "\tassign Au = A; assign Bu = B;" << std::endl
            << "\tassign As = A; assign Bs = B;" << std::endl << std::endl
            << "\tfunction automatic logic [PROD_W-1:0] mul(input int i, input int j);" << std::endl
            << "\t\tif (SIGNED)" << std::endl
            << "\t\t\tmul = As[i] * Bs[j];" << std::endl
            << "\t\telse" << std::endl
            << "\t\t\tmul = Au[i] * Bu[j];" << std::endl
            << "\tendfunction" << std::endl << std::endl << std::endl;

        // Actual Meat and Potatoes
        uint64_t a { 0 }, b { 0 }, c { 0 };
        char op;
        for (; c < maxCount; c++) {
            bool first = true; // TODO: is this the best way to do this?
            iss.clear();
            std::getline(inputFile, line);
            iss.str(line);
            
            outputFile
                << "\tassign C[" << c << "] = ";
            while (iss >> a >> op >> b) {
                if (first) 
                    first = false;
                else
                    outputFile << " + ";

                outputFile
                    << "mul(" << a << "," << b << ")";
            } 
            outputFile << ";" << std::endl;
        }

        outputFile << std::endl << "endmodule" << std::endl;

        outputFile.close();
        std::cout << "Finish." << std::endl;
    } else {
        std::cerr << "Error: could not open: "
            << (!inputFile.is_open() ? inputFileName + " " : "")
            << (!outputFile.is_open() ? outputFileName + " " : "")
            << ".txt file" << std::endl;
    }

    return 0;
}
