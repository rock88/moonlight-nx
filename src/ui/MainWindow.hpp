#include "ContentWindow.hpp"
#pragma once

class MainWindow: public ContentWindow {
public:
    MainWindow(Widget *parent);
    
    void set_add_host_callback(const std::function<void()> &callback) { m_add_host_callback = callback; }
    
private:
    std::function<void()> m_add_host_callback;
};
