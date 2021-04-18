#include "KeyboardFrontend.hpp"
#pragma once

class DummyKeyboardFrontend: public KeyboardFrontend {
public:
    KeyboardFrontendDummy() {}
    
    void handle_keyboard() {}
    
    const KeyboardState& keyboard_state() {
        return m_keyboard_state;
    }
    
private:
    KeyboardState m_keyboard_state;
};
