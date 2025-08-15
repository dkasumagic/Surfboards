#include <iostream>
#include <type_traits>
#include "webui.hpp"
#include <string_view>

int main() {

    webui::window win;

    webui::set_default_root_folder("src/frontend");
    std::string_view url = win.start_server("index.html");
    std::cout << url << "\n";

    webui::wait();
    webui::clean();

    return 0;
}
