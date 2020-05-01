#include <nanogui/nanogui.h>
#include "ContentWindow.hpp"
#pragma once

class Application: public nanogui::Screen {
public:
    Application(nanogui::Vector2f size, nanogui::Vector2f framebuffer_size);
    
    template<typename WidgetClass, typename... Args>
    WidgetClass* push_window(const Args&... args);
    void pop_window();
    
private:
    std::vector<ContentWindow *> m_windows;
};
