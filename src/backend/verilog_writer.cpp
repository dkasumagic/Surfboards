#include <iostream>
#include <fstream>
#include <filesystem>

int main() {

    std::ofstream outputFile;
    outputFile.open("verilog.txt");

    if (outputFile.is_open()) {

        outputFile << "module\n"; // open the module for the main file.



        outputFile.close();
        std::cout << "Finish.";
    } else {
        std::cerr << "Error: could not open .txt file\n";
    }

    return 0;
}

std::string matrixReader() {



    return "0";
}

std::string adder() {

    

    return "0";
}

std::string mult() {



    return "0";
}