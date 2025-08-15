#include <iostream>
#include <fstream>
#include <cstdint>


// float32_t** createMatrix(int rows, int cols) {
//     float32_t** matrix = new float32_t*[rows];
//     for (int i = 0; i < rows; ++i) {
//         matrix[i] = new float32_t[cols];
//     }
//     return matrix;
// }


void writetofile(uint8_t size) {
    std::ofstream output;
    std::string nowsize = std::to_string(size);
    output.open("output.txt");
    output << "This is the size of the matrix: " << nowsize << std::endl;
    output.close();
}

int main() {
    // This is a placeholder for the main function.
    // The actual implementation will depend on the specific requirements of the application.
    std::cout << "Matrix Handler Initialized." << std::endl;

    std::cout << "Please enter the dimensions of the first matrix: " << std::endl;
    int rows1, cols1;
    std::cin >> rows1 >> cols1;

    std::cout << "Please enter the dimensions of the second matrix: " << std::endl;
    int rows2, cols2;
    std::cin >> rows2 >> cols2;
    
    int size = rows1 * cols2;
    writetofile(size);
}

