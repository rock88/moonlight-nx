#include <nanogui/nanogui.h>
#pragma once

class LoadingOverlay: public nanogui::Widget {
public:
    LoadingOverlay(nanogui::Widget* parent, const std::string &caption = "");
    
    void draw(NVGcontext *ctx) override;
    
    bool mouse_enter_event(const nanogui::Vector2i &p, bool enter) override;
    bool mouse_button_event(const nanogui::Vector2i &p, int button, bool down, int modifiers) override;
    bool scroll_event(const nanogui::Vector2i &p, const nanogui::Vector2f &rel) override;
    
    void dispose();
private:
    int m_icon;
};
