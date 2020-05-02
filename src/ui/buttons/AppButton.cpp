#include "AppButton.hpp"
#include "nanovg.h"

using namespace nanogui;

AppButton::AppButton(Widget* parent, APP_LIST app): Button(parent, "") {
    m_app = app;
    
    set_layout(new BoxLayout(Orientation::Vertical, Alignment::Middle));
    
    m_label = add<Label>(m_app.name);
    m_label->set_fixed_width(220);
}

void AppButton::draw(NVGcontext *ctx) {
    Button::draw(ctx);
    
    nvgSave(ctx);
    nvgFillColor(ctx, Color(0, 0, 0, 100));
    nvgBeginPath(ctx);
    nvgRect(ctx, m_pos.x(), m_pos.y(), m_label->width(), m_label->height());
    nvgFill(ctx);
    nvgRestore(ctx);
    
    Widget::draw(ctx);
}
