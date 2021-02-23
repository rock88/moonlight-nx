#include "Singleton.hpp"
#include <switch.h>
#pragma once

class Application;
struct GLFWwindow;

struct MouseState {
    int x;
    int y;
    double scroll_y;
    bool l_pressed;
    bool m_pressed;
    bool r_pressed;
    
    bool is_equal(MouseState& other) {
        return x == other.x && y == other.y && scroll_y == other.scroll_y &&
               l_pressed == other.l_pressed && m_pressed == other.m_pressed &&
               r_pressed == other.r_pressed;
    }
};

class MouseController: public Singleton<MouseController> {
public:
    void init(GLFWwindow* window);
    void handle_mouse();
    void draw_cursor(Application* app);
    
    MouseState mouse_state() const {
        return m_mouse_state;
    }
    
    bool hid_mouse_is_used() const {
        return m_hid_mouse_is_used;
    }
    
    void set_draw_cursor_for_hid_mouse(bool draw_cursor_for_hid_mouse) {
        m_draw_cursor_for_hid_mouse = draw_cursor_for_hid_mouse;
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
    bool set_new_mouse_state(MouseState& state);
};
