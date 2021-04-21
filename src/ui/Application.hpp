#include <nanogui/nanogui.h>
#pragma once

class Application: public nanogui::Screen {
public:
    Application(nanogui::Vector2f size, nanogui::Vector2f framebuffer_size);
    
    void push_widget(Widget* widget);
    
    template<typename WidgetClass, typename... Args>
    WidgetClass* push_window(const Args&... args) {
        auto window = new WidgetClass(this, args...);
        push_widget(window);
        return window;
    }
    
    void pop_window();
    
    void resize_callback_event(int width, int height, int fb_width, int fb_height) override;
    void gamepad_button_callback_event(int jid, int button, int action) override;
    void gamepad_analog_callback_event(int jid, int axis, float value) override;
    
    void perform_layout();
    
private:
    
    std::vector<Widget *> m_windows;
};
