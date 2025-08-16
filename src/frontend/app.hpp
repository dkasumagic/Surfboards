#ifndef APP_HEADER_H
#define APP_HEADER_H

#include <iostream>
#include "webui.hpp"

void workflow(webui::window::event *e);

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
