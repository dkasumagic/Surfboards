#ifndef APP_HEADER_H
#define APP_HEADER_H

#include <iostream>
#include "webui.hpp"
#include "../backend/matrixhandler.hpp"
#include "../backend/verilog_writer.hpp"

class App {
public:
  webui::window win;

  App();
  ~App();

private:
  const int port = 5000;
  const std::string root_folder = "src/frontend";
  const std::string root_file = "index.html";
};

#endif // !APP_HEADER_H
