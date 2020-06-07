#include "AppListWindow.hpp"
#include "LoadingOverlay.hpp"
#include "AppButton.hpp"
#include "StreamWindow.hpp"

using namespace nanogui;

AppListWindow::AppListWindow(Widget *parent, const std::string &address): ContentWindow(parent, "Applications"), m_address(address) {
    set_left_pop_button();
    
    container()->set_layout(new GridLayout(Orientation::Horizontal, 5, Alignment::Minimum, 30, 18));
}

void AppListWindow::window_appear() {
    reload();
}

void AppListWindow::reload() {
    clean_container();
    clean_right_title_buttons();
    
    auto loader = add<LoadingOverlay>();
    
    GameStreamClient::client()->connect(m_address, [this, loader](auto result) {
        if (result.isSuccess()) {
            auto currentGame = result.value().currentGame;
            
            if (currentGame != 0) {
                set_right_title_button(FA_TIMES, [this] {
                    this->close_game();
                });
            }
            
            GameStreamClient::client()->applist(m_address, [this, loader, currentGame](auto result) {
                loader->dispose();
                
                if (result.isSuccess()) {
                    PAPP_LIST app = result.value();
                    
                    while (app != NULL) {
                        auto button = container()->add<AppButton>(m_address, *app, currentGame);
                        button->set_callback([this, app] {
                            run_game(app->id);
                        });
                        app = app->next;
                    }
                    perform_layout();
                } else {
                    screen()->add<MessageDialog>(MessageDialog::Type::Warning, "Error", result.error());
                }
            });
        } else {
            loader->dispose();
            screen()->add<MessageDialog>(MessageDialog::Type::Warning, "Error", result.error());
        }
    });
}

void AppListWindow::run_game(int app_id) {
    push<StreamWindow>(m_address, app_id);
}

void AppListWindow::close_game() {
    auto loader = add<LoadingOverlay>();
    
    GameStreamClient::client()->quit(m_address, [this, loader](auto result) {
        loader->dispose();
        
        if (result.isSuccess()) {
            reload();
        } else {
            screen()->add<MessageDialog>(MessageDialog::Type::Warning, "Error", result.error());
        }
    });
}
