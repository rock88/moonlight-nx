#include "MainWindow.hpp"
#include "AddHostWindow.hpp"
#include "AddHostButton.hpp"
#include "HostButton.hpp"
#include "GameStreamClient.hpp"
#include "LoadingOverlay.hpp"
#include "AppListWindow.hpp"
#include "SettingsWindow.hpp"
#include "Settings.hpp"

using namespace nanogui;

MainWindow::MainWindow(Widget *parent): ContentWindow(parent, "Moonlight") {
    set_box_layout(Orientation::Horizontal, Alignment::Minimum);
    
    set_right_title_button(FA_COG, [this] {
        push<SettingsWindow>();
    });
}

void MainWindow::window_appear() {
    reload();
}

void MainWindow::reload() {
    clean_container();
    
    for (auto host: Settings::settings()->hosts()) {
        auto button = container()->add<HostButton>(host);
        button->set_fixed_size(Size(200, 200));
        button->set_callback([this, button] {
            if (button->is_active()) {
                if (button->is_paired()) {
                    push<AppListWindow>(button->host());
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
    
    auto button = container()->add<AddHostButton>();
    button->set_fixed_size(Size(200, 200));
    button->set_callback([this] {
        push<AddHostWindow>();
    });
    
    perform_layout();
}
