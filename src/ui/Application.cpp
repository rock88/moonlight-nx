#include "Application.hpp"
#include "MainWindow.hpp"
#include "AddHostWindow.hpp"
#include "StreamWindow.hpp"
#include <nanogui/opengl.h>

using namespace nanogui;

extern int moonlight_exit;

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
        
        if (auto w = dynamic_cast<ContentWindow *>(m_windows.back())) {
            w->window_appear();
        }
    }
}

void Application::resize_callback_event(int width, int height, int fb_width, int fb_height) {
    for (auto window: m_windows) {
        window->set_size({ width, height });
        window->set_fixed_size({ width, height });
        window->perform_layout(nvg_context());
    }
    Screen::resize_callback_event(width, height, fb_width, fb_height);
    perform_layout();
}

void Application::gamepad_button_callback_event(int jid, int button, int action) {
    if (!m_windows.empty()) {
        m_windows.back()->gamepad_button_event(jid, button, action);
    }
    
    if (action && button == NANOGUI_GAMEPAD_BUTTON_START && m_windows.size() == 1) {
        moonlight_exit = 1;
    }
}

void Application::gamepad_analog_callback_event(int jid, int axis, float value) {
    if (!m_windows.empty()) {
        m_windows.back()->gamepad_analog_event(jid, axis, value);
    }
}

void Application::draw_contents() {
    Screen::draw_contents();
    
    if (auto w = dynamic_cast<StreamWindow *>(m_windows.back())) {
        w->draw_contents();
    }
}

void Application::perform_layout() {
    m_focus_path.clear();
    Screen::perform_layout();
}
