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
    bool r_pressed;
};

class MouseController: public Singleton<MouseController> {
public:
    void init(GLFWwindow* window);
    void handle_mouse();
    void draw_cursor(Application *app);
    
    const MouseState& mouse_state() {
        return m_state;
    }
    
    const bool is_dirty() {
        return m_is_dirty;
    }
    
private:
    MouseState m_state = {0};
    HidMouseState m_hid_mouse_state = {0};
    bool m_is_dirty = false;
    bool m_has_hid_mouse = false;
    bool m_draw_cursor_for_hid_mouse = false;
    double m_last_touch_y = 0;
    
    void handle_mouse_move(int x, int y);
    void handle_mouse_buttons(int button, int action, int modifiers);
    void handle_mouse_scroll(int x, int y);
    bool set_new_mouse_state(MouseState state);
};
