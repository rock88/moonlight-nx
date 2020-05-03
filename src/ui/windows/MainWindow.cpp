#include "MainWindow.hpp"
#include "AddHostWindow.hpp"
#include "HostButton.hpp"
#include "GameStreamClient.hpp"
#include "LoadingOverlay.hpp"
#include "AppListWindow.hpp"

using namespace nanogui;

MainWindow::MainWindow(Widget *parent): ContentWindow(parent, "Moonlight") {
    set_box_layout(Orientation::Horizontal, Alignment::Minimum);
    
    reload();
}

void MainWindow::reload() {
    for (auto child: container()->children()) {
        container()->remove_child(child);
    }
    
    for (auto host: GameStreamClient::client()->hosts()) {
        auto button = container()->add<HostButton>(host);
        button->set_fixed_size(Size(100, 100));
        button->set_callback([this, button] {
            if (button->is_active()) {
                if (button->is_paired()) {
                    application()->push_window<AppListWindow>(button->host());
                } else {
                    auto loader = add<LoadingOverlay>("Pairing... (Enter 0000)");
                    
                    GameStreamClient::client()->pair(button->host(), "0000", [this, loader](auto result){
                        loader->dispose();
                        
                        if (result.isSuccess()) {
                            reload();
                        } else {
                            screen()->add<MessageDialog>(MessageDialog::Type::Information, "Error", result.error());
                        }
                    });
                }
            } else {
                screen()->add<MessageDialog>(MessageDialog::Type::Information, "Error", "Innactive host...");
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
    
    screen()->perform_layout();
}
