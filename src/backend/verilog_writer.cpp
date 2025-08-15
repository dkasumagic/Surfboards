// Takes the matrixhandler output and produces valid verilog code.
#include <iostream>
#include <fstream>
#include <filesystem>

int main() {
    std::ofstream outputFile { "output/verilog.txt" };
    // outputFile.open;

    if (outputFile.is_open()) {

        outputFile << "yo\n"; // open the module for the main file.

        outputFile.close();
        std::cout << "Finish." << std::endl;
    } else {
        std::cerr << "Error: could not open .txt file\n";
    }

    return 0;
}
