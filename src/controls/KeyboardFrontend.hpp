#include <vector>
#pragma once

// TODO: Move glfw stuff out...
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

class KeyboardFrontend {
public:
    virtual void handle_keyboard() = 0;
    virtual const KeyboardState& keyboard_state() = 0;
};
