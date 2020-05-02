#include "ContentWindow.hpp"
#include "Server.hpp"
#pragma once

class AppListWindow: public ContentWindow {
public:
    AppListWindow(Widget *parent, SERVER_DATA data);
    
    void run_game(int id);

private:
    SERVER_DATA m_data;
    PAPP_LIST m_app_list;
};
