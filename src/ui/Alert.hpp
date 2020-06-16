#include <nanogui/nanogui.h>
#pragma once

class Alert: public nanogui::Widget {
public:
    Alert(nanogui::Widget* parent, const std::string &title, const std::string &message, bool add_ok_button = true);
    
    void perform_layout(NVGcontext *ctx) override;
    void draw(NVGcontext *ctx) override;
    
    void add_button(const std::string &caption, const std::function<void()> &callback = nullptr);
    
    bool mouse_enter_event(const nanogui::Vector2i &p, bool enter) override;
    bool mouse_button_event(const nanogui::Vector2i &p, int button, bool down, int modifiers) override;
    bool scroll_event(const nanogui::Vector2i &p, const nanogui::Vector2f &rel) override;
    
    void dispose();
    
private:
    nanogui::Widget* m_container;
    nanogui::Widget* m_button_container;
};
