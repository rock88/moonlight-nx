#include "KeyboardFrontend.hpp"
#pragma once

struct GLFWwindow;

class KeyboardFrontendSwitch: public KeyboardFrontend {
public:
    KeyboardFrontendSwitch(GLFWwindow* window);
    
    void handle_keyboard();
    
    const KeyboardState& keyboard_state() {
        return m_keyboard_state;
    }
    
private:
    KeyboardState m_keyboard_state;
    std::vector<bool> m_hid_keyboard_state;
    
    void handle_key(int key, int scancode, int action, int mods);
    void handle_char(unsigned int input);
};
