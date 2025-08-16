#ifndef APP_HEADER_H
#define APP_HEADER_H

#include <iostream>
#include "webui.hpp"

void inputMatrix(webui::window::event *e) {
    long long r1 = e->get_int();
    long long c1 = e->get_int(1);
    long long r2 = e->get_int(2);
    long long c2 = e->get_int(3);
    std::cout << "hi\n";

    bool valid = r1 > 0 && c1 > 0 && r2 > 0 && c2 > 0;
    if (!valid) {
        std::cerr << "Invalid matrix dimensions received.\n";
        e->return_string("error: invalid dimensions");
        return;
    }

    std::cout << "A: " << r1 << "x" << c1
                << " | B: " << r2 << "x" << c2 << std::endl;

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


  ~App() {
    webui::clean();
  }

private:
  const int port = 5000;
  const std::string root_folder = "src/frontend";
  const std::string root_file = "index.html";

};




#endif // !APP_HEADER_H
