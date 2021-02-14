#include "Singleton.hpp"
#include <vector>
#pragma once

struct GLFWwindow;

struct KeyCodes {
    KeyCodes(short glfw_keycode = 0, short glfw_char_code = 0, short glfw_shift_char_code = 0, short moonlight_keycode = 0);
    
    short glfw_keycode;
    short glfw_char_code;
    short glfw_shift_char_code;
    
    short moonlight_keycode;
};

struct KeyState {
    KeyCodes codes;
    bool is_pressed = false;
};

struct KeyboardState {
    KeyboardState();
    
    std::vector<KeyState> keys;
    int glfw_modifiers = 0;
    short moonlight_modifiers = 0;
};

class KeyboardController: public Singleton<KeyboardController> {
public:
    void init(GLFWwindow* window);
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
