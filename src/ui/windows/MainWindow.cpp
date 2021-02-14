#include "MainWindow.hpp"
#include "AddHostWindow.hpp"
#include "AddHostButton.hpp"
#include "HostButton.hpp"
#include "GameStreamClient.hpp"
#include "LoadingOverlay.hpp"
#include "AppListWindow.hpp"
#include "SettingsWindow.hpp"
#include "Settings.hpp"
#include "InputSettingsWindow.hpp"
#include "Alert.hpp"
#include "nanovg.h"

using namespace nanogui;

MainWindow::MainWindow(Widget *parent): ContentWindow(parent, "Moonlight") {
    set_box_layout(Orientation::Vertical, Alignment::Minimum);
    
    set_right_title_button(FA_SYNC, [this] {
        this->reload();
    });
    set_right_title_button(FA_GAMEPAD, [this] {
        push<InputSettingsWindow>(0, "Default");
    });
    set_right_title_button(FA_COG, [this] {
        push<SettingsWindow>();
    });
}

void MainWindow::window_appear() {
    reload();
}

void MainWindow::reload() {
    clean_container();
    
    container()->add<Label>("* For better performance use 5GHz WiFi, latest system firmware (11.0.0/11.0.1) and Atmosphère (0.18.0)");
    container()->add<Widget>()->set_fixed_height(6);
    
    auto button_container = container()->add<Widget>();
    button_container->set_layout(new BoxLayout(Orientation::Horizontal, Alignment::Minimum, 0, 10));
    
    for (auto host: Settings::settings()->hosts()) {
        auto button = button_container->add<HostButton>(host);
        button->set_fixed_size(Size(200, 200));
        button->set_callback([this, button] {
            if (button->is_active()) {
                if (button->is_paired()) {
                    push<AppListWindow>(button->host().address);
                } else {
                    auto loader = add<LoadingOverlay>("Pairing... (Enter 0000)");
                    
                    GameStreamClient::instance().pair(button->host().address, "0000", [this, loader](auto result){
                        loader->dispose();
                        
                        if (result.isSuccess()) {
                            reload();
                        } else {
                            screen()->add<Alert>("Error", result.error());
                        }
                    });
                }
            } else {
                auto alert = screen()->add<Alert>("Error", "Innactive host...");
                
                if (!button->host().mac.empty()) {
                    alert->add_button("Wake Up", [this, button] {
                        wake_up_host(button->host());
                    });
                }
                
                alert->add_button("Delete", [this, button] {
                    Settings::settings()->remove_host(button->host());
                    reload();
                });
            }
        });
    }
    
    auto button = button_container->add<AddHostButton>();
    button->set_fixed_size(Size(200, 200));
    button->set_callback([this] {
        push<AddHostWindow>();
    });
    
    perform_layout();
}

void MainWindow::draw(NVGcontext *ctx) {
    ContentWindow::draw(ctx);
    
    nvgSave(ctx);
    
    nvgFillColor(ctx, Color(255, 255, 255, 200));
    nvgFontSize(ctx, 20);
    nvgFontFace(ctx, "sans");
    
    int text_width = nvgTextBounds(ctx, 0, 0, MOONLIGHT_VERSION, NULL, NULL);
    nvgText(ctx, width() - text_width - 8, height() - 8, MOONLIGHT_VERSION, NULL);
    
    nvgRestore(ctx);
}

void MainWindow::wake_up_host(const Host &host) {
    auto loader = add<LoadingOverlay>("Sending Wake Up...");
    
    GameStreamClient::instance().wake_up_host(host, [this, loader](auto result) {
        loader->dispose();
        
        if (result.isSuccess()) {
            reload();
        } else {
            screen()->add<Alert>("Error", result.error());
        }
    });
}
