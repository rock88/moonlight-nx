#include "Alert.hpp"
#include "nanovg.h"

using namespace nanogui;

Alert::Alert(Widget* parent, const std::string &title, const std::string &message, bool add_ok_button): Widget(parent->screen()) {
    set_fixed_size(parent->screen()->size());
    
    m_container = add<Widget>();
    m_container->set_layout(new BoxLayout(Orientation::Vertical, Alignment::Middle, 10, 20));
    
    m_container->add<Label>(title)->set_font_size(40);
    m_container->add<Label>(message)->set_font_size(32);
    
    m_button_container = m_container->add<Widget>();
    m_button_container->set_layout(new BoxLayout(Orientation::Vertical, Alignment::Middle, 0, 10));
    
    screen()->perform_layout();
    
    if (add_ok_button) {
        add_button("OK");
    }
}

void Alert::perform_layout(NVGcontext *ctx) {
    set_fixed_size(screen()->size());
    
    m_button_container->set_fixed_width(screen()->width() / 2);
    m_container->set_fixed_width(screen()->width() / 2);
    
    for (auto child: m_container->children()) {
        child->set_fixed_width(m_container->fixed_width() - 40);
    }
    for (auto child: m_button_container->children()) {
        child->set_fixed_width(m_container->fixed_width() - 20);
    }
    m_button_container->perform_layout(ctx);
    m_container->perform_layout(ctx);
    Widget::perform_layout(ctx);
    
    m_container->set_position(Vector2i(screen()->width() / 4, screen()->height() / 2 - m_container->height() / 2));
}

void Alert::draw(NVGcontext *ctx) {
    nvgSave(ctx);
    
    // Draw bg
    nvgFillColor(ctx, Color(0, 0, 0, 100));
    nvgBeginPath(ctx);
    nvgRect(ctx, 0, 0, width(), height());
    nvgFill(ctx);
    
    // Draw container bg
    nvgFillColor(ctx, Color(48, 48, 48, 255));
    nvgBeginPath(ctx);
    nvgRect(ctx, m_container->absolute_position().x(), m_container->absolute_position().y(), m_container->width(), m_container->height());
    nvgFill(ctx);
    
    // Draw header
    nvgFillColor(ctx, Color(62, 78, 184, 255));
    nvgBeginPath(ctx);
    nvgRect(ctx, m_container->absolute_position().x(), m_container->absolute_position().y(), m_container->width(), 60);
    nvgFill(ctx);
    
    nvgRestore(ctx);
    
    Widget::draw(ctx);
}

void Alert::add_button(const std::string &caption, const std::function<void()> &callback) {
    auto button = m_button_container->add<Button>(caption);
    button->set_fixed_width(m_container->fixed_width() - 20);
    button->set_fixed_height(48);
    button->set_callback([this, callback] {
        dispose();
        
        if (callback != nullptr) {
            callback();
        }
    });
    screen()->perform_layout();
}

bool Alert::mouse_enter_event(const nanogui::Vector2i &p, bool enter) {
    return true;
}

bool Alert::mouse_button_event(const nanogui::Vector2i &p, int button, bool down, int modifiers) {
    Widget::mouse_button_event(p, button, down, modifiers);
    return true;
}

bool Alert::scroll_event(const nanogui::Vector2i &p, const nanogui::Vector2f &rel) {
    return true;
}

void Alert::dispose() {
    auto m_screen = screen();
    m_screen->update_focus(nullptr);
    m_screen->remove_child(this);
    m_screen->perform_layout();
}
