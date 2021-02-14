#include "StreamControlsController.hpp"
#include "Limelight.h"
#include "Logger.hpp"
#include <math.h>

void StreamControlsController::send_to_stream(int width, int height) {
    // Mouse
    auto mouse_state = MouseController::instance().mouse_state();
    
    if (mouse_state.x != m_mouse_state.x || mouse_state.y != m_mouse_state.y) {
        LiSendMousePositionEvent(m_mouse_state.x, m_mouse_state.y, width, height);
    }
    
    if (mouse_state.l_pressed != m_mouse_state.l_pressed) {
        LiSendMouseButtonEvent(mouse_state.l_pressed ? BUTTON_ACTION_PRESS : BUTTON_ACTION_RELEASE, BUTTON_LEFT);
    }
    
    if (mouse_state.r_pressed != m_mouse_state.r_pressed) {
        LiSendMouseButtonEvent(mouse_state.r_pressed ? BUTTON_ACTION_PRESS : BUTTON_ACTION_RELEASE, BUTTON_RIGHT);
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
}
