#include "MouseFrontend.hpp"
#include <switch.h>
#pragma once

struct GLFWwindow;

class SwitchMouseFrontend: public MouseFrontend {
public:
    SwitchMouseFrontend(GLFWwindow* window);
    
    void handle_mouse();
    
    MouseState mouse_state() const {
        return m_mouse_state;
    }
    
    bool hid_mouse_is_used() const {
        return m_hid_mouse_is_used;
    }
    
private:
    MouseState m_mouse_state = {0};
    HidMouseState m_hid_mouse_state = {0};
    bool m_hid_mouse_is_used = false;
    bool m_draw_cursor_for_hid_mouse = true;
    double m_last_touch_y = 0;
    
    void handle_mouse_move(double x, double y);
    void handle_mouse_buttons(int button, int action, int modifiers);
    void handle_mouse_scroll(int x, int y);
};
