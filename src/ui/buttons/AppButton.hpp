#include <nanogui/nanogui.h>
#include "GameStreamClient.hpp"
#pragma once

class AppButton: public nanogui::Button {
public:
    AppButton(Widget* parent, const std::string &address, AppInfo app, int current_game);
    
    bool gamepad_button_event(int jid, int button, int action) override;
    
    void set_fixed_size(const nanogui::Vector2i &fixed_size);
    void draw(NVGcontext *ctx) override;
    
private:
    std::string m_address;
    AppInfo m_app;
    nanogui::Label* m_label;
};
