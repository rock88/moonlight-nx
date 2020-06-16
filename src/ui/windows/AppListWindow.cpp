#include "AppListWindow.hpp"
#include "LoadingOverlay.hpp"
#include "AppButton.hpp"
#include "StreamWindow.hpp"
#include "GamepadMapper.hpp"
#include "Alert.hpp"

using namespace nanogui;

AppListWindow::AppListWindow(Widget *parent, const std::string &address): ContentWindow(parent, "Applications"), m_address(address) {
    set_left_pop_button();
    
    set_box_layout(Orientation::Vertical, Alignment::Minimum, 30, 10);
}

void AppListWindow::window_appear() {
    reload();
}

void AppListWindow::reload(std::function<void()> callback) {
    clean_container();
    clean_right_title_buttons();
    
    auto loader = add<LoadingOverlay>();
    
    GameStreamClient::client()->connect(m_address, [this, loader, callback](auto result) {
        if (result.isSuccess()) {
            auto currentGame = result.value().currentGame;
            
            if (currentGame != 0) {
                set_right_title_button(FA_TIMES, [this] {
                    this->close_game();
                });
            }
            
            GameStreamClient::client()->applist(m_address, [this, loader, currentGame, callback](auto result) {
                loader->dispose();
                
                if (result.isSuccess()) {
                    container()->add<Label>("* For adjust Gamepad buttons and Combo keys for a specific game, select this game with a Dpad and press Y");
                    container()->add<Widget>()->set_fixed_height(6);
                    
                    auto button_container = container()->add<Widget>();
                    button_container->set_layout(new GridLayout(Orientation::Horizontal, 5, Alignment::Minimum, 0, 18));
                    
                    PAPP_LIST app = result.value();
                    
                    while (app != NULL) {
                        auto button = button_container->add<AppButton>(m_address, *app, currentGame);
                        button->set_callback([this, app] {
                            run_game(app->id);
                        });
                        app = app->next;
                    }
                    perform_layout();
                    
                    if (callback) {
                        callback();
                    }
                } else {
                    screen()->add<Alert>("Error", result.error());
                }
            });
        } else {
            loader->dispose();
            screen()->add<Alert>("Error", result.error());
        }
    });
}

void AppListWindow::run_game(int app_id) {
    int current_app_id = GameStreamClient::client()->server_data(m_address).currentGame;
    
    if (current_app_id == 0 || current_app_id == app_id) {
        GamepadMapper::mapper()->load_gamepad_map(app_id);
        push<StreamWindow>(m_address, app_id);
    } else {
        auto alert = screen()->add<Alert>("Info", "Another game already running", false);
        alert->add_button("Resume", [this, current_app_id] {
            GamepadMapper::mapper()->load_gamepad_map(current_app_id);
            push<StreamWindow>(m_address, current_app_id);
        });
        alert->add_button("Close", [this, app_id] {
            close_game([this, app_id] {
                GamepadMapper::mapper()->load_gamepad_map(app_id);
                push<StreamWindow>(m_address, app_id);
            });
        });
    }
}

void AppListWindow::close_game(std::function<void()> callback) {
    auto loader = add<LoadingOverlay>();
    
    GameStreamClient::client()->quit(m_address, [this, loader, callback](auto result) {
        loader->dispose();
        
        if (result.isSuccess()) {
            reload(callback);
        } else {
            screen()->add<Alert>("Error", result.error());
        }
    });
}
