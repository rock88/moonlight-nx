#include "KeyboardController.hpp"
#include "Limelight.h"
#include "Logger.hpp"
#include <nanogui/opengl.h>
#include <GLFW/glfw3.h>

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

void KeyboardController::init(KeyboardFrontend* frontend) {
    m_frontend = frontend;
}

void KeyboardController::handle_keyboard() {
    m_frontend->handle_keyboard();
    
    auto state = m_frontend->keyboard_state();
    
    for (int i = 0; i < state.keys.size(); i++) {
        if (state.keys[i].is_pressed != m_keyboard_state.keys[i].is_pressed) {
            if (state.keys[i].is_pressed && state.keys[i].codes.glfw_char_code != -1) {
                if (state.glfw_modifiers & GLFW_MOD_SHIFT && state.keys[i].codes.glfw_shift_char_code != -1) {
                    nanogui::keyboard_character_event(state.keys[i].codes.glfw_shift_char_code);
                } else {
                    nanogui::keyboard_character_event(state.keys[i].codes.glfw_char_code);
                }
            }
            
            nanogui::keyboard_callback_event(state.keys[i].codes.glfw_keycode, 0, state.keys[i].is_pressed, state.glfw_modifiers);
        }
    }
    
    m_keyboard_state = state;
}
