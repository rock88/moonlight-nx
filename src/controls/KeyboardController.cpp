#include "KeyboardController.hpp"
#include "Limelight.h"
#include "Logger.hpp"
#include <nanogui/opengl.h>
#include <GLFW/glfw3.h>
#include <switch.h>

// https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
#define VK_0 0x30
#define VK_A 0x41
#define VK_F1 0x70

KeyCodes::KeyCodes(short glfw_keycode, short glfw_char_code, short glfw_shift_char_code, short moonlight_keycode):
glfw_keycode(glfw_keycode),
glfw_char_code(glfw_char_code),
glfw_shift_char_code(glfw_shift_char_code),
moonlight_keycode(moonlight_keycode) {}

KeyboardState::KeyboardState() {
    // F1 - F12
    for (int i = GLFW_KEY_F1; i <= GLFW_KEY_F12; i++) {
        keys.push_back({.codes = KeyCodes(i, -1, -1, VK_F1 + i - GLFW_KEY_F1) });
    }
    
    // A - Z
    for (int i = GLFW_KEY_A; i <= GLFW_KEY_Z; i++) {
        keys.push_back({.codes = KeyCodes(i, i + 32, i, VK_A + i - GLFW_KEY_A) });
    }
    
    // 0 - 9
    for (int i = GLFW_KEY_0; i <= GLFW_KEY_9; i++) {
        keys.push_back({.codes = KeyCodes(i, i, -1, VK_0 + i - GLFW_KEY_0) });
    }
    
    keys.push_back({.codes = KeyCodes(GLFW_KEY_SPACE, GLFW_KEY_SPACE, -1, 0x20) });
    keys.push_back({.codes = KeyCodes(GLFW_KEY_APOSTROPHE, GLFW_KEY_APOSTROPHE, -1, 0xDE) });
    keys.push_back({.codes = KeyCodes(GLFW_KEY_COMMA, GLFW_KEY_COMMA, -1, 0xBC) });
    keys.push_back({.codes = KeyCodes(GLFW_KEY_MINUS, GLFW_KEY_MINUS, -1, 0xBD) });
    keys.push_back({.codes = KeyCodes(GLFW_KEY_PERIOD, GLFW_KEY_PERIOD, -1, 0xBE) });
    keys.push_back({.codes = KeyCodes(GLFW_KEY_SLASH, GLFW_KEY_SLASH, -1, 0xBF) });
    keys.push_back({.codes = KeyCodes(GLFW_KEY_SEMICOLON, GLFW_KEY_SEMICOLON, -1, 0xBA) });
    keys.push_back({.codes = KeyCodes(GLFW_KEY_EQUAL, GLFW_KEY_EQUAL, -1, 0xBB) });
    keys.push_back({.codes = KeyCodes(GLFW_KEY_LEFT_BRACKET, GLFW_KEY_LEFT_BRACKET, -1, 0xDB) });
    keys.push_back({.codes = KeyCodes(GLFW_KEY_RIGHT_BRACKET, GLFW_KEY_RIGHT_BRACKET, -1, 0xDD) });
    keys.push_back({.codes = KeyCodes(GLFW_KEY_BACKSLASH, GLFW_KEY_BACKSLASH, -1, 0xDC) });
    keys.push_back({.codes = KeyCodes(GLFW_KEY_GRAVE_ACCENT, GLFW_KEY_GRAVE_ACCENT, -1, 0xC0) });
    keys.push_back({.codes = KeyCodes(GLFW_KEY_WORLD_1, GLFW_KEY_GRAVE_ACCENT, -1, 0xC0) }); // Ok?
    
    keys.push_back({.codes = KeyCodes(GLFW_KEY_ESCAPE, -1, -1, 0x1B) });
    keys.push_back({.codes = KeyCodes(GLFW_KEY_ENTER, -1, -1, 0x0D) });
    keys.push_back({.codes = KeyCodes(GLFW_KEY_TAB, -1, -1, 0x09) });
    keys.push_back({.codes = KeyCodes(GLFW_KEY_BACKSPACE, -1, -1, 0x08) });
    keys.push_back({.codes = KeyCodes(GLFW_KEY_CAPS_LOCK, -1, -1, 0x14) });
    
    keys.push_back({.codes = KeyCodes(GLFW_KEY_LEFT_SHIFT, -1, -1, 0xA0) });
    keys.push_back({.codes = KeyCodes(GLFW_KEY_LEFT_CONTROL, -1, -1, 0xA2) });
    keys.push_back({.codes = KeyCodes(GLFW_KEY_LEFT_ALT, -1, -1, 0xA4) });
    keys.push_back({.codes = KeyCodes(GLFW_KEY_LEFT_SUPER, -1, -1, 0x5B) });
    
    keys.push_back({.codes = KeyCodes(GLFW_KEY_RIGHT_SHIFT, -1, -1, 0xA1) });
    keys.push_back({.codes = KeyCodes(GLFW_KEY_RIGHT_CONTROL, -1, -1, 0xA3) });
    keys.push_back({.codes = KeyCodes(GLFW_KEY_RIGHT_ALT, -1, -1, 0xA5) });
    keys.push_back({.codes = KeyCodes(GLFW_KEY_RIGHT_SUPER, -1, -1, 0x5C) });
    
    keys.push_back({.codes = KeyCodes(GLFW_KEY_LEFT, -1, -1, 0x25) });
    keys.push_back({.codes = KeyCodes(GLFW_KEY_RIGHT, -1, -1, 0x27) });
    keys.push_back({.codes = KeyCodes(GLFW_KEY_UP, -1, -1, 0x26) });
    keys.push_back({.codes = KeyCodes(GLFW_KEY_DOWN, -1, -1, 0x28) });
}

static int find_glfw_key_index(int key, KeyboardState& state) {
    for (int i = 0; i < state.keys.size(); i++) {
        if (state.keys[i].codes.glfw_keycode == key) {
            return i;
        }
    }
    return -1;
}

static int find_switch_key_index(int key, KeyboardState& state) {
    if (KBD_A <= key && key <= KBD_Z) {
        return find_glfw_key_index(key - KBD_A + GLFW_KEY_A, state);
    } else if (KBD_1 <= key && key <= KBD_9) {
        return find_glfw_key_index(key - KBD_1 + GLFW_KEY_1, state);
    } else if (KBD_F1 <= key && key <= KBD_F12) {
        return find_glfw_key_index(key - KBD_F1 + GLFW_KEY_F1, state);
    }
    
    switch (key) {
        case KBD_0: return find_glfw_key_index(GLFW_KEY_0, state);
        case KBD_SPACE: return find_glfw_key_index(GLFW_KEY_SPACE, state);
        case KBD_APOSTROPHE: return find_glfw_key_index(GLFW_KEY_APOSTROPHE, state);
        case KBD_COMMA: return find_glfw_key_index(GLFW_KEY_COMMA, state);
        case KBD_MINUS: return find_glfw_key_index(GLFW_KEY_MINUS, state);
        case KBD_DOT: return find_glfw_key_index(GLFW_KEY_PERIOD, state);
        case KBD_SLASH: return find_glfw_key_index(GLFW_KEY_SLASH, state);
        case KBD_SEMICOLON: return find_glfw_key_index(GLFW_KEY_SEMICOLON, state);
        case KBD_EQUAL: return find_glfw_key_index(GLFW_KEY_EQUAL, state);
        case KBD_LEFTBRACE: return find_glfw_key_index(GLFW_KEY_LEFT_BRACKET, state);
        case KBD_RIGHTBRACE: return find_glfw_key_index(GLFW_KEY_RIGHT_BRACKET, state);
        case KBD_BACKSLASH: return find_glfw_key_index(GLFW_KEY_BACKSLASH, state);
        case KBD_GRAVE: return find_glfw_key_index(GLFW_KEY_GRAVE_ACCENT, state);
        case KBD_ESC: return find_glfw_key_index(GLFW_KEY_ESCAPE, state);
        case KBD_ENTER: return find_glfw_key_index(GLFW_KEY_ENTER, state);
        case KBD_TAB: return find_glfw_key_index(GLFW_KEY_TAB, state);
        case KBD_BACKSPACE: return find_glfw_key_index(GLFW_KEY_BACKSPACE, state);
        case KBD_CAPSLOCK: return find_glfw_key_index(GLFW_KEY_CAPS_LOCK, state);
        case KBD_LEFTSHIFT: return find_glfw_key_index(GLFW_KEY_LEFT_SHIFT, state);
        case KBD_LEFTCTRL: return find_glfw_key_index(GLFW_KEY_LEFT_CONTROL, state);
        case KBD_LEFTALT: return find_glfw_key_index(GLFW_KEY_LEFT_ALT, state);
        case KBD_LEFTMETA: return find_glfw_key_index(GLFW_KEY_LEFT_SUPER, state);
        case KBD_RIGHTSHIFT: return find_glfw_key_index(GLFW_KEY_RIGHT_SHIFT, state);
        case KBD_RIGHTCTRL: return find_glfw_key_index(GLFW_KEY_RIGHT_CONTROL, state);
        case KBD_RIGHTALT: return find_glfw_key_index(GLFW_KEY_RIGHT_ALT, state);
        case KBD_RIGHTMETA: return find_glfw_key_index(GLFW_KEY_RIGHT_SUPER, state);
        case KBD_LEFT: return find_glfw_key_index(GLFW_KEY_LEFT, state);
        case KBD_RIGHT: return find_glfw_key_index(GLFW_KEY_RIGHT, state);
        case KBD_UP: return find_glfw_key_index(GLFW_KEY_UP, state);
        case KBD_DOWN: return find_glfw_key_index(GLFW_KEY_DOWN, state);
        default: return -1;
    }
}

void KeyboardController::init(GLFWwindow* window) {
    hidInitializeKeyboard();
    
    glfwSetKeyCallback(window, [](GLFWwindow *w, int key, int scancode, int action, int mods) {
        #ifndef __SWITCH__
        KeyboardController::instance().handle_key(key, scancode, action, mods);
        #endif
    });
    
    glfwSetCharCallback(window, [](GLFWwindow *w, auto input) {
        #ifndef __SWITCH__
        KeyboardController::instance().handle_char(input);
        #endif
    });
    
    m_hid_keyboard_state.assign(256, false);
}

void KeyboardController::handle_keyboard() {
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
                        mods |= GLFW_MOD_SHIFT;
                    }
                    
                    if (state.modifiers & HidKeyboardModifier_Control) {
                        mods |= GLFW_MOD_CONTROL;
                    }
                    
                    if ((state.modifiers & HidKeyboardModifier_LeftAlt) || (state.modifiers & HidKeyboardModifier_RightAlt)) {
                        mods |= GLFW_MOD_ALT;
                    }
                    
                    handle_key(m_keyboard_state.keys[index].codes.glfw_keycode, 0, is_pressed ? GLFW_PRESS : GLFW_RELEASE, mods);
                } else {
                    Logger::info("Keyboard", "Unhandled HID keyboard key: 0x%X", i);
                }
                
                m_hid_keyboard_state[i] = is_pressed;
            }
        }
    }
}

void KeyboardController::handle_key(int key, int scancode, int action, int mods) {
    nanogui::keyboard_callback_event(key, scancode, action, mods);
    
    int index = find_glfw_key_index(key, m_keyboard_state);
    
    if (index >= 0) {
        m_keyboard_state.keys[index].is_pressed = action != 0;
    } else {
        Logger::info("Keyboard", "Unhandled GLFW key: %i", key);
    }
    
    m_keyboard_state.glfw_modifiers = mods;
    
    short moonlight_modifiers = 0;
    
    if (mods & GLFW_MOD_SHIFT) {
        moonlight_modifiers |= MODIFIER_SHIFT;
    }
    
    if (mods & GLFW_MOD_CONTROL) {
        moonlight_modifiers |= MODIFIER_CTRL;
    }
    
    if (mods & GLFW_MOD_ALT) {
        moonlight_modifiers |= MODIFIER_ALT;
    }
    
    if (mods & GLFW_MOD_SUPER) {
        moonlight_modifiers |= MODIFIER_META;
    }
    
    m_keyboard_state.moonlight_modifiers = moonlight_modifiers;
}

void KeyboardController::handle_char(unsigned int input) {
    nanogui::keyboard_character_event(input);
}
