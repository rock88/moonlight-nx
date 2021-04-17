#include "GamepadController.hpp"
#include "Limelight.h"
#include <nanogui/nanogui.h>
#include <nanogui/opengl.h>

static inline void gamepad_callback(short& buttonFlags, int moonlight_button, int nanogui_button, int index, int* nanogui_gamepad_state) {
    if (buttonFlags & moonlight_button) {
        if (nanogui_gamepad_state[index] == 0) {
            nanogui_gamepad_state[index] = 1;
            nanogui::gamepad_button_callback_event(0, nanogui_button, 1);
        }
    } else {
        if (nanogui_gamepad_state[index] == 1) {
            nanogui_gamepad_state[index] = 0;
            nanogui::gamepad_button_callback_event(0, nanogui_button, 0);
        }
    }
}

void GamepadController::init(GamepadFrontend* frontend) {
    m_frontend = frontend;
}

void GamepadController::handle_gamepad() {
    m_frontend->handle_gamepad();
    
    auto state = m_frontend->gamepad_state();
    
    if (state.buttonFlags != m_gamepad_state.buttonFlags) {
        gamepad_callback(state.buttonFlags, A_FLAG, NANOGUI_GAMEPAD_BUTTON_A, 0, m_nanogui_gamepad_state);
        gamepad_callback(state.buttonFlags, B_FLAG, NANOGUI_GAMEPAD_BUTTON_B, 1, m_nanogui_gamepad_state);
        gamepad_callback(state.buttonFlags, X_FLAG, NANOGUI_GAMEPAD_BUTTON_X, 2, m_nanogui_gamepad_state);
        gamepad_callback(state.buttonFlags, Y_FLAG, NANOGUI_GAMEPAD_BUTTON_Y, 3, m_nanogui_gamepad_state);
        gamepad_callback(state.buttonFlags, LEFT_FLAG, NANOGUI_GAMEPAD_BUTTON_DPAD_LEFT, 4, m_nanogui_gamepad_state);
        gamepad_callback(state.buttonFlags, RIGHT_FLAG, NANOGUI_GAMEPAD_BUTTON_DPAD_RIGHT, 5, m_nanogui_gamepad_state);
        gamepad_callback(state.buttonFlags, UP_FLAG, NANOGUI_GAMEPAD_BUTTON_DPAD_UP, 6, m_nanogui_gamepad_state);
        gamepad_callback(state.buttonFlags, DOWN_FLAG, NANOGUI_GAMEPAD_BUTTON_DPAD_DOWN, 7, m_nanogui_gamepad_state);
        gamepad_callback(state.buttonFlags, BACK_FLAG, NANOGUI_GAMEPAD_BUTTON_BACK, 8, m_nanogui_gamepad_state);
        gamepad_callback(state.buttonFlags, PLAY_FLAG, NANOGUI_GAMEPAD_BUTTON_START, 9, m_nanogui_gamepad_state);
        gamepad_callback(state.buttonFlags, LB_FLAG, NANOGUI_GAMEPAD_BUTTON_LEFT_BUMPER, 10, m_nanogui_gamepad_state);
        gamepad_callback(state.buttonFlags, RB_FLAG, NANOGUI_GAMEPAD_BUTTON_RIGHT_BUMPER, 11, m_nanogui_gamepad_state);
        gamepad_callback(state.buttonFlags, LS_CLK_FLAG, NANOGUI_GAMEPAD_BUTTON_LEFT_THUMB, 12, m_nanogui_gamepad_state);
        gamepad_callback(state.buttonFlags, RS_CLK_FLAG, NANOGUI_GAMEPAD_BUTTON_RIGHT_THUMB, 13, m_nanogui_gamepad_state);
    }
    
    if (state.leftStickX != m_gamepad_state.leftStickX) {
        nanogui::gamepad_analog_callback_event(0, NANOGUI_GAMEPAD_AXIS_LEFT_X, (float)state.leftStickX / 0x7FFF);
    }
    
    if (state.leftStickY != m_gamepad_state.leftStickY) {
        nanogui::gamepad_analog_callback_event(0, NANOGUI_GAMEPAD_AXIS_LEFT_Y, (float)state.leftStickY / 0x7FFF);
    }
    
    if (state.rightStickX != m_gamepad_state.rightStickX) {
        nanogui::gamepad_analog_callback_event(0, NANOGUI_GAMEPAD_AXIS_RIGHT_X, (float)state.rightStickX / 0x7FFF);
    }
    
    if (state.rightStickY != m_gamepad_state.rightStickY) {
        nanogui::gamepad_analog_callback_event(0, NANOGUI_GAMEPAD_AXIS_RIGHT_Y, (float)state.rightStickY / 0x7FFF);
    }
    
    if (state.leftTrigger != m_gamepad_state.leftTrigger) {
        nanogui::gamepad_analog_callback_event(0, NANOGUI_GAMEPAD_AXIS_LEFT_TRIGGER, state.leftTrigger ? 1 : 0);
    }
    
    if (state.rightTrigger != m_gamepad_state.rightTrigger) {
        nanogui::gamepad_analog_callback_event(0, NANOGUI_GAMEPAD_AXIS_RIGHT_TRIGGER, state.rightTrigger ? 1 : 0);
    }
    
    m_gamepad_state = state;
}

void GamepadController::handle_rumple(unsigned short controller, unsigned short low_freq_motor, unsigned short high_freq_motor) {
    m_frontend->handle_rumple(controller, low_freq_motor, high_freq_motor);
}

void GamepadController::stop_rumple() {
    m_frontend->stop_rumple();
}
