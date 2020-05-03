#include "ContentWindow.hpp"
#include "GameStreamClient.hpp"
#pragma once

class AddHostWindow: public ContentWindow {
public:
    AddHostWindow(Widget *parent);
    
    void set_host_added_callback(const std::function<void(SERVER_DATA)> &callback) { m_host_added_callback = callback; }
    
private:
    std::function<void(SERVER_DATA)> m_host_added_callback;
};
