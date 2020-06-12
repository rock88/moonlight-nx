#include "ContentWindow.hpp"
#include "GameStreamClient.hpp"
#pragma once

class AppListWindow: public ContentWindow {
public:
    AppListWindow(Widget *parent, const std::string &address);
    
    void window_appear() override;

private:
    void reload(std::function<void()> callback = NULL);
    void run_game(int app_id);
    void close_game(std::function<void()> callback = NULL);
    
    std::string m_address;
    PAPP_LIST m_app_list;
};
