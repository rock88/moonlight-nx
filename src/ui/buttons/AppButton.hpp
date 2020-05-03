#include <nanogui/nanogui.h>
#include "GameStreamClient.hpp"
#pragma once

class AppButton: public nanogui::Button {
public:
    AppButton(Widget* parent, APP_LIST app, int currentGame);
    
    void draw(NVGcontext *ctx) override;
    
private:
    APP_LIST m_app;
    nanogui::Label* m_label;
};
