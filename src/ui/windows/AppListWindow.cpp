#include "AppListWindow.hpp"
#include "LoadingOverlay.hpp"
#include "AppButton.hpp"

using namespace nanogui;

AppListWindow::AppListWindow(Widget *parent, SERVER_DATA data): ContentWindow(parent, "Appications"), m_data(data) {
    set_left_pop_button();
    
    auto loader = add<LoadingOverlay>();
    
    Server::server()->applist(m_data, [this, loader](auto result) {
        loader->dispose();
        
        if (result.isSuccess()) {
            container()->set_layout(new GridLayout(Orientation::Horizontal, 5, Alignment::Minimum, 10, 10));
            
            m_app_list = result.value();
            
            PAPP_LIST app = m_app_list;
            
            while (app != NULL) {
                auto button = container()->add<AppButton>(*app);
                button->set_fixed_size(Size(220, 100));
                button->set_callback([this, app] {
                    run_game(app->id);
                });
                app = app->next;
            }
            screen()->perform_layout();
        } else {
            screen()->add<MessageDialog>(MessageDialog::Type::Information, "Error", result.error());
        }
    });
}

void AppListWindow::run_game(int id) {
    printf("Run %i\n", id);
}
