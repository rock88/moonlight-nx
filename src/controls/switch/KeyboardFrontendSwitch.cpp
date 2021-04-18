#include "KeyboardFrontendSwitch.hpp"
#include "Logger.hpp"
#include "Limelight.h"
#include <switch.h>
#include <nanogui/opengl.h>
#include <GLFW/glfw3.h>

static KeyboardFrontendSwitch* m_frontend = NULL;

static inline int find_glfw_key_index(int key, KeyboardState& state) {
    for (int i = 0; i < state.keys.size(); i++) {
        if (state.keys[i].codes.glfw_keycode == key) {
            return i;
        }
    }
    return -1;
}

static inline int find_switch_key_index(int key, KeyboardState& state) {
    if (KBD_A <= key && key <= KBD_Z) {
        return find_glfw_key_index(key - KBD_A + NANOGUI_KEY_A, state);
    } else if (KBD_1 <= key && key <= KBD_9) {
        return find_glfw_key_index(key - KBD_1 + NANOGUI_KEY_1, state);
    } else if (KBD_F1 <= key && key <= KBD_F12) {
        return find_glfw_key_index(key - KBD_F1 + NANOGUI_KEY_F1, state);
    }
    
    switch (key) {
        case KBD_0: return find_glfw_key_index(NANOGUI_KEY_0, state);
        case KBD_SPACE: return find_glfw_key_index(NANOGUI_KEY_SPACE, state);
        case KBD_APOSTROPHE: return find_glfw_key_index(NANOGUI_KEY_APOSTROPHE, state);
        case KBD_COMMA: return find_glfw_key_index(NANOGUI_KEY_COMMA, state);
        case KBD_MINUS: return find_glfw_key_index(NANOGUI_KEY_MINUS, state);
        case KBD_DOT: return find_glfw_key_index(NANOGUI_KEY_PERIOD, state);
        case KBD_SLASH: return find_glfw_key_index(NANOGUI_KEY_SLASH, state);
        case KBD_SEMICOLON: return find_glfw_key_index(NANOGUI_KEY_SEMICOLON, state);
        case KBD_EQUAL: return find_glfw_key_index(NANOGUI_KEY_EQUAL, state);
        case KBD_LEFTBRACE: return find_glfw_key_index(NANOGUI_KEY_LEFT_BRACKET, state);
        case KBD_RIGHTBRACE: return find_glfw_key_index(NANOGUI_KEY_RIGHT_BRACKET, state);
        case KBD_BACKSLASH: return find_glfw_key_index(NANOGUI_KEY_BACKSLASH, state);
        case KBD_GRAVE: return find_glfw_key_index(NANOGUI_KEY_GRAVE_ACCENT, state);
        case KBD_ESC: return find_glfw_key_index(NANOGUI_KEY_ESCAPE, state);
        case KBD_ENTER: return find_glfw_key_index(NANOGUI_KEY_ENTER, state);
        case KBD_TAB: return find_glfw_key_index(NANOGUI_KEY_TAB, state);
        case KBD_BACKSPACE: return find_glfw_key_index(NANOGUI_KEY_BACKSPACE, state);
        case KBD_CAPSLOCK: return find_glfw_key_index(NANOGUI_KEY_CAPS_LOCK, state);
        case KBD_LEFTSHIFT: return find_glfw_key_index(NANOGUI_KEY_LEFT_SHIFT, state);
        case KBD_LEFTCTRL: return find_glfw_key_index(NANOGUI_KEY_LEFT_CONTROL, state);
        case KBD_LEFTALT: return find_glfw_key_index(NANOGUI_KEY_LEFT_ALT, state);
        case KBD_LEFTMETA: return find_glfw_key_index(NANOGUI_KEY_LEFT_SUPER, state);
        case KBD_RIGHTSHIFT: return find_glfw_key_index(NANOGUI_KEY_RIGHT_SHIFT, state);
        case KBD_RIGHTCTRL: return find_glfw_key_index(NANOGUI_KEY_RIGHT_CONTROL, state);
        case KBD_RIGHTALT: return find_glfw_key_index(NANOGUI_KEY_RIGHT_ALT, state);
        case KBD_RIGHTMETA: return find_glfw_key_index(NANOGUI_KEY_RIGHT_SUPER, state);
        case KBD_LEFT: return find_glfw_key_index(NANOGUI_KEY_LEFT, state);
        case KBD_RIGHT: return find_glfw_key_index(NANOGUI_KEY_RIGHT, state);
        case KBD_UP: return find_glfw_key_index(NANOGUI_KEY_UP, state);
        case KBD_DOWN: return find_glfw_key_index(NANOGUI_KEY_DOWN, state);
        default: return -1;
    }
}

KeyboardFrontendSwitch::KeyboardFrontendSwitch(GLFWwindow* window) {
    m_frontend = this;
    
    hidInitializeKeyboard();
    
    glfwSetKeyCallback(window, [](GLFWwindow *w, int key, int scancode, int action, int mods) {
        #ifndef __SWITCH__
        m_frontend->handle_key(key, scancode, action, mods);
        #endif
    });
    
    glfwSetCharCallback(window, [](GLFWwindow *w, auto input) {
        #ifndef __SWITCH__
        m_frontend->handle_char(input);
        #endif
    });
    
    m_hid_keyboard_state.assign(256, false);
}

void KeyboardFrontendSwitch::handle_keyboard() {
    HidKeyboardState state;
    
    if (hidGetKeyboardStates(&state, 1)) {
        for (int i = 0; i < 256; ++i) {
            auto is_pressed = (state.keys[i / 64] & (1ul << (i % 64))) != 0;
            
            if (m_hid_keyboard_state[i] != is_pressed) {
                int index = find_switch_key_index(i, m_keyboard_state);
                
                if (index != -1) {
                    if (is_pressed) {
                        if ((state.modifiers & HidKeyboardModifier_Shift) && m_keyboard_state.keys[index].codes.glfw_shift_char_code != -1) {
                            handle_char(m_keyboard_state.keys[index].codes.glfw_shift_char_code);
                        } else if (m_keyboard_state.keys[index].codes.glfw_char_code != -1) {
                            handle_char(m_keyboard_state.keys[index].codes.glfw_char_code);
                        }
                    }
                    
                    int mods = 0;
                    
                    if (state.modifiers & HidKeyboardModifier_Shift) {
                        mods |= NANOGUI_MOD_SHIFT;
                    }
                    
                    if (state.modifiers & HidKeyboardModifier_Control) {
                        mods |= NANOGUI_MOD_CONTROL;
                    }
                    
                    if ((state.modifiers & HidKeyboardModifier_LeftAlt) || (state.modifiers & HidKeyboardModifier_RightAlt)) {
                        mods |= NANOGUI_MOD_ALT;
                    }
                    
                    handle_key(m_keyboard_state.keys[index].codes.glfw_keycode, 0, is_pressed ? NANOGUI_PRESS : NANOGUI_RELEASE, mods);
                } else {
                    Logger::info("Keyboard", "Unhandled HID keyboard key: 0x%X", i);
                }
                
                m_hid_keyboard_state[i] = is_pressed;
            }
        }
    }
}

void KeyboardFrontendSwitch::handle_key(int key, int scancode, int action, int mods) {
    int index = find_glfw_key_index(key, m_keyboard_state);
    
    if (index >= 0) {
        m_keyboard_state.keys[index].is_pressed = action != 0;
    } else {
        Logger::info("Keyboard", "Unhandled GLFW key: %i", key);
    }
    
    m_keyboard_state.glfw_modifiers = mods;
    
    short moonlight_modifiers = 0;
    
    if (mods & NANOGUI_MOD_SHIFT) {
        moonlight_modifiers |= MODIFIER_SHIFT;
    }
    
    if (mods & NANOGUI_MOD_CONTROL) {
        moonlight_modifiers |= MODIFIER_CTRL;
    }
    
    if (mods & NANOGUI_MOD_ALT) {
        moonlight_modifiers |= MODIFIER_ALT;
    }
    
    if (mods & NANOGUI_MOD_SUPER) {
        moonlight_modifiers |= MODIFIER_META;
    }
    
    m_keyboard_state.moonlight_modifiers = moonlight_modifiers;
}

void KeyboardFrontendSwitch::handle_char(unsigned int input) {
    // TODO: no needs anymore?
}
