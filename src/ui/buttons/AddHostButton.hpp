#include <nanogui/nanogui.h>
#pragma once

class AddHostButton: public nanogui::Button {
public:
    AddHostButton(nanogui::Widget* parent);
    
    void draw(NVGcontext *ctx) override;
};
