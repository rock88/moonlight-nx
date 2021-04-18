#include "KeyboardFrontend.hpp"
#pragma once

struct GLFWwindow;

class SwitchKeyboardFrontend: public KeyboardFrontend {
public:
    SwitchKeyboardFrontend(GLFWwindow* window);
    
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
