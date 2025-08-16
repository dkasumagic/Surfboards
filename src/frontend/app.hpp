#ifndef APP_HEADER_H
#define APP_HEADER_H

#include <iostream>
#include "webui.hpp"
#include "../backend/matrixhandler.hpp"
#include "../backend/verilog_writer.hpp"

void inputMatrix(webui::window::event *e) {
    long long r1 = e->get_int();
    long long c1 = e->get_int(1);
    long long r2 = e->get_int(2);
    long long c2 = e->get_int(3);
    long long q1 = e->get_int(4);
    long long q2 = e->get_int(5);
    // long long s1 = e->get_int(6); Signed bool value for from frontend
    // user choice to do svd or not.

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

    std::cout << "A: " << r1 << "x" << c1
                << " | B: " << r2 << "x" << c2 
                << " | C: " << r1 << "x" << c2 
                << " | Q" << q1 << "." << q2 << "\n";

    e->return_string("ok");
}

class App {
public:
  webui::window win;


  App()  {
    webui::set_config(folder_monitor, true);
    win.set_port(port);


    win.bind("inputMatrix", inputMatrix);

    webui::set_default_root_folder(root_folder);
    std::string_view url = win.start_server(root_file);
    std::cout << url << "\n";
    webui::wait();

  }

  // void run() {
  //   webui::wait();
  // }


  ~App() {
    webui::clean();
  }

private:
  const int port = 5000;
  const std::string root_folder = "src/frontend";
  const std::string root_file = "index.html";

};




#endif // !APP_HEADER_H
