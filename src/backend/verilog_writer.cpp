// Takes the matrixhandler output and produces valid verilog code.
#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <cstdint>

int main() {
    // HUMAN COMMENTS btw haha
    // Initalise my stuff
    const std::string inputFileName { "output/MH_output.txt" };
    const std::string outputFileName { "output/verilog.txt" };
    std::ifstream inputFile { inputFileName };
    std::ofstream outputFile { outputFileName };
    std::string line;
   
    if (inputFile.is_open() && outputFile.is_open()) {
        // Get the size of my matrix
        uint64_t row, col;
        std::getline(inputFile, line);
        std::istringstream iss(line);
        iss >> row >> col;

        // Write some preamble
        outputFile 
            << "`timescale 1ns/1ps" << std::endl << std::endl
            << "module surfboard #(" << std::endl
            << "\tparameter int W = 32," << std::endl // TODO: eventually make this a user option
            << "\tparameter bit SIGNED = 0" << std::endl // TODO: eventually make this a user option
            << ")(" << std::endl
            << "\tinput  logic [W-1:0] A [0:" << (row*col)-1 << "]," << std::endl
            << "\tinput  logic [W-1:0] B [0:" << (row*col)-1 << "]," << std::endl
            << "\toutput logic [SUMW-1:0] C [0:" << (row*col)-1 << "]" << std::endl
            << ");" << std::endl
            << "\tlocalparam int PROD_W = 2*W;" << std::endl
            << "\tlocalparam int SUMW   = PROD_W + 1;" << std::endl << std::endl
            << "\tlogic signed   [W-1:0] As [0:" << (row*col)-1 << "];" << std::endl //TODO HERE DOWN
            << "\tlogic signed   [W-1:0] Bs [0:" << (row*col)-1 << "];" << std::endl
            << "\tlogic          [W-1:0] Au [0:" << (row*col)-1 << "];" << std::endl
            << "\tlogic          [W-1:0] Bu [0:" << (row*col)-1 << "];" << std::endl << std::endl
            << "\tassign Au = A; assign Bu = B;" << std::endl
            << "\tassign As = A; assign Bs = B;" << std::endl << std::endl
            << "\tfunction automatic logic [PROD_W-1:0] mul(input int i, input int j);" << std::endl
            << "\t\tif (SIGNED)" << std::endl
            << "\t\t\tmul = As[i] * Bs[j];" << std::endl
            << "\t\telse" << std::endl
            << "\t\t\tmul = Au[i] * Bu[j];" << std::endl
            << "\tendfunction" << std::endl;


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
