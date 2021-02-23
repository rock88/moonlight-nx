#include "StreamControlsController.hpp"
#include "GamepadMapper.hpp"
#include "Limelight.h"
#include "Logger.hpp"
#include "Settings.hpp"
#include <math.h>
#include <unistd.h>
#include <nanogui/nanogui.h>

void StreamControlsController::handle_rumple(unsigned short controller, unsigned short low_freq_motor, unsigned short high_freq_motor) {
    GamepadController::instance().handle_rumple(controller, low_freq_motor, high_freq_motor);
}

void StreamControlsController::stop_rumple() {
    GamepadController::instance().stop_rumple();
}

void StreamControlsController::send_to_stream(int width, int height) {
    // Mouse
    auto mouse_state = MouseController::instance().mouse_state();
    
    if (MouseController::instance().hid_mouse_is_used()) {
        if (mouse_state.x != m_mouse_state.x || mouse_state.y != m_mouse_state.y) {
            LiSendMousePositionEvent(m_mouse_state.x, m_mouse_state.y, width, height);
        }
        
        if (mouse_state.l_pressed != m_mouse_state.l_pressed) {
            LiSendMouseButtonEvent(mouse_state.l_pressed ? BUTTON_ACTION_PRESS : BUTTON_ACTION_RELEASE, BUTTON_LEFT);
        }
        
        if (mouse_state.m_pressed != m_mouse_state.m_pressed) {
            LiSendMouseButtonEvent(mouse_state.m_pressed ? BUTTON_ACTION_PRESS : BUTTON_ACTION_RELEASE, BUTTON_MIDDLE);
        }
        
        if (mouse_state.r_pressed != m_mouse_state.r_pressed) {
            LiSendMouseButtonEvent(mouse_state.r_pressed ? BUTTON_ACTION_PRESS : BUTTON_ACTION_RELEASE, BUTTON_RIGHT);
        }
    } else {
        auto gamepad = GamepadController::instance().gamepad_state();
        
        if (Settings::instance().click_by_tap()) {
            if (m_mouse_state.x != mouse_state.x || m_mouse_state.y != mouse_state.y) {
                LiSendMousePositionEvent(mouse_state.x, mouse_state.y, width, height);
            }
            
            if ((gamepad.buttonFlags & RB_FLAG) || gamepad.rightTrigger > 0) {
                if (mouse_state.l_pressed) {
                    m_mouse_is_released = false;
                    LiSendMouseButtonEvent(BUTTON_ACTION_PRESS, BUTTON_RIGHT);
                } else if (!m_mouse_is_released) {
                    m_mouse_is_released = true;
                    LiSendMouseButtonEvent(BUTTON_ACTION_RELEASE, BUTTON_RIGHT);
                }
            } else {
                if (mouse_state.l_pressed) {
                    m_mouse_is_released = false;
                    LiSendMouseButtonEvent(BUTTON_ACTION_PRESS, BUTTON_LEFT);
                } else if (!m_mouse_is_released) {
                    m_mouse_is_released = true;
                    LiSendMouseButtonEvent(BUTTON_ACTION_RELEASE, BUTTON_LEFT);
                }
            }
        } else {
            if (mouse_state.l_pressed && !m_mouse_is_pressed) {
                m_mouse_is_pressed = true;
                m_mouse_state.x = mouse_state.x;
                m_mouse_state.y = mouse_state.y;
            } else if (!mouse_state.l_pressed && m_mouse_is_pressed) {
                m_mouse_is_pressed = false;
            }
            
            bool move_mouse = gamepad.leftTrigger == 0 && gamepad.rightTrigger == 0;
            
            if (move_mouse && m_mouse_is_pressed && (m_mouse_state.x != mouse_state.x || m_mouse_state.y != mouse_state.y)) {
                int relative_mouse_x = mouse_state.x - m_mouse_state.x;
                int relative_mouse_y = mouse_state.y - m_mouse_state.y;
                LiSendMouseMoveEvent(relative_mouse_x, relative_mouse_y);
            }
        }
        
        if ((gamepad.buttonFlags & LB_FLAG) || gamepad.leftTrigger > 0) {
            if (mouse_state.l_pressed) {
                m_mouse_is_released = false;
                LiSendMouseButtonEvent(BUTTON_ACTION_PRESS, BUTTON_LEFT);
            } else if (!m_mouse_is_released) {
                m_mouse_is_released = true;
                LiSendMouseButtonEvent(BUTTON_ACTION_RELEASE, BUTTON_LEFT);
            }
        } else if ((gamepad.buttonFlags & RB_FLAG) || gamepad.rightTrigger > 0) {
            if (mouse_state.l_pressed) {
                m_mouse_is_released = false;
                LiSendMouseButtonEvent(BUTTON_ACTION_PRESS, BUTTON_RIGHT);
            } else if (!m_mouse_is_released) {
                m_mouse_is_released = true;
                LiSendMouseButtonEvent(BUTTON_ACTION_RELEASE, BUTTON_RIGHT);
            }
        }
    }
    
    if (mouse_state.scroll_y != m_mouse_state.scroll_y && m_mouse_state.scroll_y != 0) {
        LiSendHighResScrollEvent(m_mouse_state.scroll_y > 0 ? fmax(m_mouse_state.scroll_y, 1) : fmin(m_mouse_state.scroll_y, -1));
    }
    
    m_mouse_state = mouse_state;
    
    // Keyboard
    auto keyboard_state = KeyboardController::instance().keyboard_state();
    
    for (int i = 0; i < keyboard_state.keys.size(); i++) {
        if (keyboard_state.keys[i].is_pressed != m_keyboard_state.keys[i].is_pressed) {
            LiSendKeyboardEvent(
                keyboard_state.keys[i].codes.moonlight_keycode,
                keyboard_state.keys[i].is_pressed ? KEY_ACTION_DOWN : KEY_ACTION_UP,
                keyboard_state.moonlight_modifiers
            );
        }
    }
    
    m_keyboard_state = keyboard_state;
    
    // Gamepad
    auto gamepad = GamepadController::instance().gamepad_state();
    auto mapped_gamepad = GamepadMapper::instance().map(gamepad);
    
    if (GamepadMapper::instance().gamepad_combo_is_enabled(gamepad, GamepadComboGuide)) {
        mapped_gamepad.buttonFlags |= SPECIAL_FLAG;
    }
    
    if (!m_gamepad_state.is_equal(mapped_gamepad)) {
        m_gamepad_state = mapped_gamepad;
        
        LiSendControllerEvent(
          mapped_gamepad.buttonFlags,
          mapped_gamepad.leftTrigger,
          mapped_gamepad.rightTrigger,
          mapped_gamepad.leftStickX,
          mapped_gamepad.leftStickY,
          mapped_gamepad.rightStickX,
          mapped_gamepad.rightStickY
        );
    }
    
    // Combo
    if (!m_press_escape && GamepadMapper::instance().gamepad_combo_is_enabled(gamepad, GamepadComboEscape)) {
        m_press_escape = true;
        LiSendKeyboardEvent(0x1B, KEY_ACTION_DOWN, 0);
    } else if (m_press_escape && !GamepadMapper::instance().gamepad_combo_is_enabled(gamepad, GamepadComboEscape)) {
        m_press_escape = false;
        LiSendKeyboardEvent(0x1B, KEY_ACTION_UP, 0);
    }
    
    if (!m_press_win_key && GamepadMapper::instance().gamepad_combo_is_enabled(gamepad, GamepadComboWinKey)) {
        m_press_win_key = true;
        LiSendKeyboardEvent(0x5B, KEY_ACTION_DOWN, 0);
    } else if (m_press_win_key && !GamepadMapper::instance().gamepad_combo_is_enabled(gamepad, GamepadComboWinKey)) {
        m_press_win_key = false;
        LiSendKeyboardEvent(0x5B, KEY_ACTION_UP, 0);
    }
    
    if (!m_press_alt_enter && GamepadMapper::instance().gamepad_combo_is_enabled(gamepad, GamepadComboAltEnter)) {
        m_press_alt_enter = true;
        LiSendKeyboardEvent(0xA4, KEY_ACTION_DOWN, 0);
        LiSendKeyboardEvent(0x0D, KEY_ACTION_DOWN, MODIFIER_ALT);
    } else if (m_press_alt_enter && !GamepadMapper::instance().gamepad_combo_is_enabled(gamepad, GamepadComboAltEnter)) {
        m_press_alt_enter = false;
        LiSendKeyboardEvent(0xA4, KEY_ACTION_UP, 0);
        LiSendKeyboardEvent(0x0D, KEY_ACTION_UP, MODIFIER_ALT);
    }
    
    if (!m_press_shift_tab && GamepadMapper::instance().gamepad_combo_is_enabled(gamepad, GamepadComboShiftTab)) {
        m_press_shift_tab = true;
        LiSendKeyboardEvent(0xA0, KEY_ACTION_DOWN, 0);
        LiSendKeyboardEvent(0x09, KEY_ACTION_DOWN, MODIFIER_SHIFT);
    } else if (m_press_shift_tab && !GamepadMapper::instance().gamepad_combo_is_enabled(gamepad, GamepadComboShiftTab)) {
        m_press_shift_tab = false;
        LiSendKeyboardEvent(0xA0, KEY_ACTION_UP, 0);
        LiSendKeyboardEvent(0x09, KEY_ACTION_UP, MODIFIER_SHIFT);
    }
}

bool StreamControlsController::should_exit() const {
    return GamepadMapper::instance().gamepad_combo_is_enabled(GamepadController::instance().gamepad_state(), GamepadComboExit);
}

bool StreamControlsController::should_exit_and_close() const {
    return GamepadMapper::instance().gamepad_combo_is_enabled(GamepadController::instance().gamepad_state(), GamepadComboExitAndClose);
}

bool StreamControlsController::should_show_stats() const {
    return GamepadMapper::instance().gamepad_combo_is_enabled(GamepadController::instance().gamepad_state(), GamepadComboShowStats);
}
