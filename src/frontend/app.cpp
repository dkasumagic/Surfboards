#include "app.hpp"
#include "matrixhandler.hpp"
#include "verilog_writer.hpp"
#include "synthesis_engine.hpp"

void workflow(webui::window::event *e){
  long long r1 = e->get_int();
  long long c1 = e->get_int(1);
  long long r2 = e->get_int(2);
  long long c2 = e->get_int(3);
  long long q1 = e->get_int(4);
  long long q2 = e->get_int(5);
  long long s1 = e->get_int(6);
  long long useSVD = e->get_int(7);
  long long singularValuesA = e->get_int(8); // Singular values for Matrix A
  long long singularValuesB = e->get_int(9); // Singular values for Matrix B

  std::cout << "Executing...\n";
  bool valid = r1 > 0 && c1 > 0 && r2 > 0 && c2 > 0;
  if (!valid) {
    std::cerr << "Invalid matrix dimensions received.\n";
    e->return_string("error: invalid dimensions");
    return;
  }
  
  std::string matrixMathBlueprint = "./build/matrix_math.txt";
  std::string verilogOutput = "./build/verilog_out.sv";
  
  generateMatrixMathBlueprint(matrixMathBlueprint, r1, c1, r2, c2);
  std::cout << "Matrix generated...\n";

  writeVerilogFile(matrixMathBlueprint, verilogOutput, (q1+q2));
  std::cout << "Verilog file generated...\n";

  SynthesisResult synthRes = runSynthesis("build/verilog_out.sv", "src/backend/synth.ys");
  std::cout 
    << "Synthesis Engine returned code " << synthRes.code << std::endl
    << synthRes.message << std::endl;

  std::cout << "A: " << r1 << "x" << c1
    << " | B: " << r2 << "x" << c2 
    << " | C: " << r1 << "x" << c2 
    << " | Q" << q1 << "." << q2
    << " | SVD: " << (useSVD ? "enabled" : "disabled");
  
  if (useSVD) {
    std::cout << " (A: " << singularValuesA << " values, B: " << singularValuesB << " values)";
  }
  
  std::cout << "\n\n" << "Success!\n\n";

  e->return_string("ok");
}

App::App(){
  webui::set_config(folder_monitor, true);
  win.set_port(port);
  
  win.bind("inputMatrix", workflow);
  
  webui::set_default_root_folder(root_folder);
  std::string_view url = win.start_server(root_file);
  std::cout << url << "\n";
  
  webui::wait();
}

App::~App(){
  webui::clean();
}