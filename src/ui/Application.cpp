#include "Application.hpp"
#include "MainWindow.hpp"
#include "AddHostWindow.hpp"
#include "StreamWindow.hpp"

using namespace nanogui;

Application::Application(Vector2f size, Vector2f framebuffer_size): Screen(size, framebuffer_size) {
    theme()->m_standard_font_size = 24;
    theme()->m_button_font_size = 24;
    theme()->m_text_box_font_size = 34;
    
    theme()->m_text_color = Color(255, 255, 255, 255);
    theme()->m_disabled_text_color = Color(255, 255, 255, 220);
    
    theme()->m_border_light = Color(0, 0, 0, 0);
    theme()->m_border_dark = Color(0, 0, 0, 0);
    theme()->m_button_gradient_top_focused = Color(255, 255, 255, 10);
    theme()->m_button_gradient_bot_focused = Color(255, 255, 255, 10);
    theme()->m_button_gradient_top_unfocused = Color(255, 255, 255, 5);
    theme()->m_button_gradient_bot_unfocused = Color(255, 255, 255, 5);
    theme()->m_button_gradient_top_pushed = Color(255, 255, 255, 30);
    theme()->m_button_gradient_bot_pushed = Color(255, 255, 255, 30);
    
    push_window<MainWindow>();
}

void Application::push_widget(Widget* widget) {
    for (auto window: m_windows) {
        if (window->visible()) {
            if (auto w = dynamic_cast<ContentWindow *>(window)) {
                w->window_disappear();
            }
        }
        
        window->set_visible(false);
    }
    
    widget->set_size(size());
    widget->set_fixed_size(size());
    m_windows.push_back(widget);
    
    if (auto w = dynamic_cast<ContentWindow *>(widget)) {
        w->window_appear();
    }
    
    update_focus(widget);
    perform_layout();
}

void Application::pop_window() {
    if (m_windows.size() > 1) {
        auto last = m_windows.back();
        m_windows.pop_back();
        update_focus(NULL);
        
        if (auto w = dynamic_cast<ContentWindow *>(last)) {
            w->window_disappear();
        }
        
        remove_child(last);
        m_windows.back()->set_visible(true);
        update_focus(m_windows.back());
        perform_layout();
    }
    
    if (auto w = static_cast<ContentWindow *>(m_windows.front())) {
        w->window_appear();
    }
}
