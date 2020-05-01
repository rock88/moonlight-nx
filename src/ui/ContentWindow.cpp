#include "ContentWindow.hpp"
#include "Application.hpp"
#include "nanovg.h"

using namespace nanogui;

ContentWindow::ContentWindow(Widget *parent, const std::string& title): Widget(parent) {
    set_layout(new BoxLayout(Orientation::Vertical, Alignment::Middle, 0, 30));
    set_size(parent->size());
    set_fixed_size(parent->size());
    
    auto title_container = add<Widget>();
    title_container->set_fixed_size(Size(parent->width() - 40, 80));
    title_container->set_layout(new BoxLayout(Orientation::Horizontal, Alignment::Middle, 0, 10));
    
    m_title_button_container = title_container->add<Widget>();
    m_title_button_container->set_layout(new BoxLayout(Orientation::Horizontal));
    
    m_title_label = title_container->add<Label>(title);
    m_title_label->set_font_size(30);
    
    m_scroll = add<VScrollPanel>();
    m_scroll->set_fixed_size(Size(parent->width() - 60, parent->height() - 140));
    m_container = m_scroll->add<Widget>();
}

void ContentWindow::draw(NVGcontext *ctx) {
    nvgSave(ctx);
    
    // Draw bg
    nvgFillColor(ctx, theme()->m_window_fill_focused);
    nvgBeginPath(ctx);
    nvgRect(ctx, 0, 0, width(), height());
    nvgFill(ctx);
    
    // Draw separator
    nvgFillColor(ctx, theme()->m_text_color);
    nvgBeginPath(ctx);
    nvgRect(ctx, 0, 80, width(), 2);
    nvgFill(ctx);
    
    // Draw content
    nvgFillColor(ctx, theme()->m_disabled_text_color);
    nvgBeginPath(ctx);
    nvgRect(ctx, m_scroll->position().x(), m_scroll->position().y(), m_scroll->width(), m_scroll->height());
    nvgFill(ctx);
    
    nvgRestore(ctx);
    
    Widget::draw(ctx);
}

void ContentWindow::set_left_title_button(int icon, const std::function<void()> &callback) {
    auto button = m_title_button_container->add<Button>("", icon);
    button->set_fixed_size(Size(40, 40));
    button->set_icon_extra_scale(2);
    button->set_callback([callback] {
        callback();
    });
}

void ContentWindow::pop() {
    auto app = static_cast<Application *>(screen());
    app->pop_window();
}
