#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <cstdint>

int svd_verilog_writer(std::string& inputFile, std::string& outputFile, int numBits, int n, int m, int r) { 

    std::ifstream readFile { inputFile };
    std::ofstream writeFile { outputFile };
    std::string line;

    if (readFile.is_open() && writeFile.is_open()) {

        int finMatSize = n*r;
        int uaMatSize = n*n;
        int daMatSize = n*m;
        int vaMatSize = m*m;
        int ubMatSize = m*m;
        int dbMatSize = m*r;
        int vbMatSize = r*r;

        bool sign = true; // TODO: generalise this please. Talk to Howard.

 
        // Write the preamble
        // writeFile 
        //     << "`timescale 1ns/1ps\n\n"
        //     << "module surfboard #(\n"
        //     << "\tparameter int W = " << numBits << ",\n"
        //     << "\tparameter bit SIGNED = " << signBit
        //     << "\n)(\n"
        //     << "\tinput  logic [W-1:0] A [0:" << maxIndex << "],\n"
        //     << "\tinput  logic [W-1:0] B [0:" << maxIndex << "],\n"
        //     << "\toutput logic [2*W-1:0] C [0:" << maxIndex << "]\n"
        //     << ");\n"
        //     << "\tlogic signed   [W-1:0] As [0:" << maxIndex << "];\n"
        //     << "\tlogic signed   [W-1:0] Bs [0:" << maxIndex << "];\n"
        //     << "\tlogic          [W-1:0] Au [0:" << maxIndex << "];\n"
        //     << "\tlogic          [W-1:0] Bu [0:" << maxIndex << "];\n\n"
        //     << "\tassign Au = A; assign Bu = B;\n"
        //     << "\tassign As = A; assign Bs = B;\n\n"
        //     << "\tfunction automatic logic [2*W-1:0] mul(input int i, input int j);\n"
        //     << "\t\tif (SIGNED)\n"
        //     << "\t\t\tmul = As[i] * Bs[j];\n"
        //     << "\t\telse\n"
        //     << "\t\t\tmul = Au[i] * Bu[j];\n"
        //     << "\tendfunction\n\n";




    }
    else {
        std::cerr << "Error: could not open file/s: "
            << (!readFile.is_open() ? inputFile : "")
            << (!writeFile.is_open() ? outputFile : "")
            << std::endl;
    }
    



    return 0;
}

std::string mult_add(std::string& inputFile) {

    std::string matrixMath;

    std::ifstream readFile { inputFile };
    std::string line;




    return matrixMath;
}

