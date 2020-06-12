#include "MainWindow.hpp"
#include "AddHostWindow.hpp"
#include "AddHostButton.hpp"
#include "HostButton.hpp"
#include "GameStreamClient.hpp"
#include "LoadingOverlay.hpp"
#include "AppListWindow.hpp"
#include "SettingsWindow.hpp"
#include "LogsWindow.hpp"
#include "Settings.hpp"
#include "InputSettingsWindow.hpp"
#include "nanovg.h"

using namespace nanogui;

MainWindow::MainWindow(Widget *parent): ContentWindow(parent, "Moonlight") {
    set_box_layout(Orientation::Vertical, Alignment::Minimum);
    
    set_right_title_button(FA_SYNC, [this] {
        this->reload();
    });
    set_right_title_button(FA_GAMEPAD, [this] {
        push<InputSettingsWindow>();
    });
    set_right_title_button(FA_COG, [this] {
        push<SettingsWindow>();
    });
    set_right_title_button(FA_FILE_ALT, [this] {
        push<LogsWindow>();
    });
}

void MainWindow::window_appear() {
    reload();
}

void MainWindow::reload() {
    clean_container();
    
    container()->add<Label>("* For better performance use 5GHz WiFi, latest system firmware (10.0.2/10.0.3) and Atmosphère (0.12.0)");
    container()->add<Widget>()->set_fixed_height(6);
    
    auto button_container = container()->add<Widget>();
    button_container->set_layout(new BoxLayout(Orientation::Horizontal, Alignment::Minimum, 0, 10));
    
    for (auto host: Settings::settings()->hosts()) {
        auto button = button_container->add<HostButton>(host);
        button->set_fixed_size(Size(200, 200));
        button->set_callback([this, button] {
            if (button->is_active()) {
                if (button->is_paired()) {
                    push<AppListWindow>(button->address());
                } else {
                    auto loader = add<LoadingOverlay>("Pairing... (Enter 0000)");
                    
                    GameStreamClient::client()->pair(button->address(), "0000", [this, loader](auto result){
                        loader->dispose();
                        
                        if (result.isSuccess()) {
                            reload();
                        } else {
                            screen()->add<MessageDialog>(MessageDialog::Type::Warning, "Error", result.error());
                        }
                    });
                }
            } else {
                auto alert = screen()->add<MessageDialog>(MessageDialog::Type::Warning, "Error", "Innactive host...", "OK", "Delete", true);
                alert->set_callback([this, button](int action) {
                    if (action) {
                        Settings::settings()->remove_host(button->address());
                        reload();
                    }
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
