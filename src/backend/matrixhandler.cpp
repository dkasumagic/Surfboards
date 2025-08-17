#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdint>
#include "matrixhandler.hpp"

MatrixIterator::MatrixIterator(int n, int m, int r) 
    : n_(n), m_(m), r_(r), i_(0), j_(0)  {}

bool MatrixIterator::hasNext() const {
    return i_ < n_;
}

std::string MatrixIterator::next_line() {
    if (!hasNext()) {
        return {}; 
    }

    std::ostringstream oss;
    for (int k = 0; k < m_; ++k) {
        oss << i_ * m_ + k << "*" << k * r_ + j_;
        if (k < m_ - 1) {
            oss << "+";
        }
    }
    oss << ";";

    ++j_;
    if (j_ >= r_) {
        j_ = 0;
        ++i_;
    }
    return oss.str();
}
     
void generateMatrixMathBlueprint(const std::string& outputFile, int rows1, int cols1, int rows2, int cols2) {
    std::ofstream output { outputFile };
    output << rows1 << " " << cols2 << ";" << std::endl;
    for (int i = 0; i < rows1; i++) {
        for (int j = 0; j < cols2; j++) {
            for (int k = 0; k < cols1; k++) { // rows2 = cols1 and k is the inbetween index
                output << std::to_string(i * cols1 + k) << "*" << std::to_string(k * cols2 + j);

                if (k < cols1 - 1) {
                    output << "+";
                } else {
                    output << ";";
                }
            }
            output << std::endl;
        }
    }
    output.close();
}

// int main() {
//     std::cout << "Matrix Handler Initialized." << std::endl;

//     // std::cout << "Please enter the dimensions of the first matrix: " << std::endl;
//     // int rows1, cols1;
//     // std::cin >> rows1 >> cols1;

//     // std::cout << "Please enter the dimensions of the second matrix: " << std::endl;
//     // int rows2, cols2;
//     // std::cin >> rows2 >> cols2;
    
//     int rows1 = 1000, cols1 = 400; // Example dimensions for the first matrix
//     int rows2 = 400, cols2 = 2000; // Example dimensions for the

//     writetofile(rows1, cols1, rows2, cols2);

//     return 0;
// }
