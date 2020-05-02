#include "MainWindow.hpp"
#include "AddHostWindow.hpp"
#include "HostButton.hpp"
#include "Server.hpp"
#include "LoadingOverlay.hpp"

using namespace nanogui;

MainWindow::MainWindow(Widget *parent): ContentWindow(parent, "Moonlight") {
    set_box_layout(Orientation::Horizontal, Alignment::Minimum);
    
    reload();
}

void MainWindow::reload() {
    for (auto child: container()->children()) {
        container()->remove_child(child);
    }
    
    for (auto host: Server::server()->hosts()) {
        auto button = container()->add<HostButton>(host);
        button->set_fixed_size(Size(100, 100));
        button->set_callback([this, button] {
            if (button->server_data().paired) {
                
            } else {
                add<LoadingOverlay>("Pairing...");
            }
        });
    }
    
    auto button = container()->add<Button>("Add Host");
    button->set_fixed_size(Size(100, 100));
    button->set_callback([this] {
        auto add_host = application()->push_window<AddHostWindow>();
        add_host->set_host_added_callback([this](auto _) {
            this->reload();
            this->application()->pop_window();
        });
    });
}
