#include <iostream>
#include "webui.hpp"

int main() {

    webui::window win;
    win.show("<html><script src=\"webui.js\"></script> Hello World from C++! </html>");
    webui::wait();

    return 0;
}
// User frontend/interface
