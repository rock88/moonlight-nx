#include "MouseFrontend.hpp"
#pragma once

class DummyMouseFrontend: public MouseFrontend {
public:
    MouseFrontendDummy() {}
    
    void handle_mouse() {}
    
    MouseState mouse_state() const {
        return m_mouse_state;
    }
    
    bool hid_mouse_is_used() const {
        return false;
    }
    
private:
    MouseState m_mouse_state = {0};
};
