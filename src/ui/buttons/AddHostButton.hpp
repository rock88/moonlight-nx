#include <nanogui/nanogui.h>
#pragma once

enum AddHostButtonType: int {
    AddHostButtonTypeFind = 0,
    AddHostButtonTypeAdd
};

class AddHostButton: public nanogui::Button {
public:
    AddHostButton(nanogui::Widget* parent, AddHostButtonType type);
    
    void draw(NVGcontext *ctx) override;

private:
    AddHostButtonType m_type;
};
