#include "MainWindow.hpp"

using namespace nanogui;

MainWindow::MainWindow(Widget *parent): ContentWindow(parent, "Moonlight") {
    set_box_layout(Orientation::Horizontal, Alignment::Minimum, 10, 10);
    
    auto button = container()->add<Button>("Add Host");
    button->set_fixed_size(Size(100, 100));
    button->set_callback([this] {
        this->m_add_host_callback();
    });
}
