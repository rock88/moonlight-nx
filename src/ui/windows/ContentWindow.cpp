#include "ContentWindow.hpp"
#include "Application.hpp"
#include "nanovg.h"

using namespace nanogui;

ContentWindow::ContentWindow(Widget *parent, const std::string& title): Widget(parent) {
    set_layout(new BoxLayout(Orientation::Vertical, Alignment::Middle, 0, 0));
    set_size(parent->size());
    set_fixed_size(parent->size());
    
    auto title_container = add<Widget>();
    title_container->set_fixed_size(Size(parent->width() - 40, 80));
    title_container->set_layout(new BoxLayout(Orientation::Horizontal, Alignment::Middle, 0, 10));
    
    m_left_title_button_container = title_container->add<Widget>();
    m_left_title_button_container->set_layout(new BoxLayout(Orientation::Horizontal));
    
    m_title_label = title_container->add<Label>(title);
    m_title_label->set_fixed_width(parent->width() - 140);
    m_title_label->set_font_size(40);
    
    m_right_title_button_container = title_container->add<Widget>();
    m_right_title_button_container->set_layout(new BoxLayout(Orientation::Horizontal));
    
    m_scroll = add<VScrollPanel>();
    m_scroll->set_fixed_size(Size(parent->width() - 60, parent->height() - 80));
    m_container = m_scroll->add<Widget>();
}

void ContentWindow::draw(NVGcontext *ctx) {
    nvgSave(ctx);
    
    // Draw bg
    nvgFillColor(ctx, Color(48, 48, 48, 255));
    nvgBeginPath(ctx);
    nvgRect(ctx, 0, 0, width(), height());
    nvgFill(ctx);
    
    // Draw header
    nvgFillColor(ctx, Color(62, 78, 184, 255));
    nvgBeginPath(ctx);
    nvgRect(ctx, 0, 0, width(), 80);
    nvgFill(ctx);
    
    // Draw separator
    nvgBeginPath(ctx);
    NVGpaint gradient = nvgLinearGradient(ctx, 0, 80, 0, 84, Color(0, 0, 0, 100), Color(0, 0, 0, 0));
    nvgFillPaint(ctx, gradient);
    nvgRect(ctx, 0, 80, width(), 4);
    nvgFill(ctx);
    
    nvgRestore(ctx);
    
    Widget::draw(ctx);
}

void ContentWindow::set_left_title_button(int icon, const std::function<void()> &callback) {
    auto button = m_left_title_button_container->add<Button>("", icon);
    button->set_fixed_size(Size(60, 60));
    button->set_icon_extra_scale(3);
    button->set_callback([callback] {
        callback();
    });
    perform_layout();
}

void ContentWindow::set_right_title_button(int icon, const std::function<void()> &callback) {
    auto button = m_right_title_button_container->add<Button>("", icon);
    button->set_fixed_size(Size(60, 60));
    button->set_icon_extra_scale(3);
    button->set_callback([callback] {
        callback();
    });
    perform_layout();
}
