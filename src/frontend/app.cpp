#include <iostream>
#include "app.hpp"
#include "../backend/verilog_writer.hpp"

int main() {

  std::cout << "test\n";
  std::string inputFileName("output.txt");
  std::string outputFileName("verilog_out.txt");
  write_verilog_file(inputFileName, outputFileName);

  App app = App();
  
  return 0;
}
