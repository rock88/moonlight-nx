#include <nanogui/nanogui.h>
#include "Server.hpp"
#pragma once

class AppButton: public nanogui::Button {
public:
    AppButton(Widget* parent, APP_LIST app);
    
    void draw(NVGcontext *ctx) override;
    
private:
    APP_LIST m_app;
    nanogui::Label* m_label;
};
