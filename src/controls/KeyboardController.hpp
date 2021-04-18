#include "Singleton.hpp"
#include "KeyboardFrontend.hpp"
#pragma once

class KeyboardController: public Singleton<KeyboardController> {
public:
    void init(KeyboardFrontend* frontend);
    void handle_keyboard();
    
    const KeyboardState& keyboard_state() {
        return m_keyboard_state;
    }
    
private:
    KeyboardFrontend* m_frontend;
    KeyboardState m_keyboard_state;
};
