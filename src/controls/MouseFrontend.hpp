#pragma once

struct MouseState {
    int x;
    int y;
    double scroll_y;
    bool l_pressed;
    bool m_pressed;
    bool r_pressed;
};

class MouseFrontend {
public:
    virtual void handle_mouse() = 0;
    virtual MouseState mouse_state() const = 0;
    virtual bool hid_mouse_is_used() const = 0;
};
