#include "Singleton.hpp"
#include "MouseFrontend.hpp"
#pragma once

class Application;

class MouseController: public Singleton<MouseController> {
public:
    void init(MouseFrontend* frontend);
    void handle_mouse();
    void draw_cursor(Application* app);
    
    MouseState mouse_state() const {
        return m_mouse_state;
    }
    
    bool hid_mouse_is_used() const {
        return m_frontend->hid_mouse_is_used();
    }
    
    void set_draw_cursor_for_hid_mouse(bool draw_cursor_for_hid_mouse) {
        m_draw_cursor_for_hid_mouse = draw_cursor_for_hid_mouse;
    }
    
private:
    MouseState m_mouse_state = {0};
    MouseFrontend* m_frontend;
    
    bool m_draw_cursor_for_hid_mouse = true;
};
