#include "ContentWindow.hpp"
#include "GameStreamClient.hpp"
#pragma once

class AppListWindow: public ContentWindow {
public:
    AppListWindow(Widget *parent, const std::string &address);
    
    void window_appear() override;
    
    void run_game(int app_id);

private:
    std::string m_address;
    PAPP_LIST m_app_list;
};
