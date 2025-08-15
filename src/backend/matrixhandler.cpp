#include <iostream>
#include <fstream>
#include <cstdint>

// Thing the app produces the textfile for the operations

// float* createMatrix(int rows, int cols) {
//     float* matrix = new float[rows * cols];
//     for (int i = 0; i < rows; ++i) {
//         for (int j = 0; j < cols; ++j) {
//             matrix[i * cols + j] = i * j;
//         }
//     }

//     return matrix;
// }


void writetofile(int rows1, int cols1, int rows2, int cols2) {
    std::ofstream output;
    output.open("output.txt");
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

int main() {
    std::cout << "Matrix Handler Initialized." << std::endl;

    // std::cout << "Please enter the dimensions of the first matrix: " << std::endl;
    // int rows1, cols1;
    // std::cin >> rows1 >> cols1;

    // std::cout << "Please enter the dimensions of the second matrix: " << std::endl;
    // int rows2, cols2;
    // std::cin >> rows2 >> cols2;
    
    int rows1 = 2, cols1 = 3; // Example dimensions for the first matrix
    int rows2 = 3, cols2 = 4; // Example dimensions for the

    writetofile(rows1, cols1, rows2, cols2);

    return 0;
}

