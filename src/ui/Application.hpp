#include <nanogui/nanogui.h>
#pragma once

#define Size(x, y) (nanogui::Vector2f((x), (y)))

class Application: public nanogui::Screen {
public:
    Application(nanogui::Vector2f size, nanogui::Vector2f framebuffer_size);
    
    template<typename WidgetClass, typename... Args>
    WidgetClass* push_window(const Args&... args) {
        for (auto window: m_windows) {
            window->set_visible(false);
        }
        
        auto window = new WidgetClass(this, args...);
        window->set_size(size());
        window->set_fixed_size(size());
        m_windows.push_back(window);
        perform_layout();
        return window;
    }
    
    void pop_window();
    
private:
    std::vector<Widget *> m_windows;
};
