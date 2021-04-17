#include "GamepadFrontendSwitch.hpp"
#include "Limelight.h"
#include <nanogui/opengl.h>
#include <GLFW/glfw3.h>
#include <map>
#include <vector>

static inline bool pad_is_equal(PadState& state, PadState& other) {
    return padGetButtons(&state) == padGetButtons(&other) &&
    padGetStickPos(&state, 0).x == padGetStickPos(&other, 0).x &&
    padGetStickPos(&state, 0).y == padGetStickPos(&other, 0).y &&
    padGetStickPos(&state, 1).x == padGetStickPos(&other, 1).x &&
    padGetStickPos(&state, 1).y == padGetStickPos(&other, 1).y;
}

static inline int pad_button_number(HidNpadButton button) {
    static std::map<HidNpadButton, int> map = {
        { HidNpadButton_A, 0 },
        { HidNpadButton_B, 1 },
        { HidNpadButton_X, 2 },
        { HidNpadButton_Y, 3 },
        { HidNpadButton_StickL, 4 },
        { HidNpadButton_StickR, 5 },
        { HidNpadButton_L, 6 },
        { HidNpadButton_R, 7 },
        { HidNpadButton_ZL, 8 },
        { HidNpadButton_ZR, 9 },
        { HidNpadButton_Plus, 10 },
        { HidNpadButton_Minus, 11 },
        { HidNpadButton_Left, 12 },
        { HidNpadButton_Up, 13 },
        { HidNpadButton_Right, 14 },
        { HidNpadButton_Down, 15 }
    };
    return map[button];
}

static inline int pad_button_to_moonlight_button(int button) {
    static std::map<int, int> map = {
        { pad_button_number(HidNpadButton_A), A_FLAG },
        { pad_button_number(HidNpadButton_B), B_FLAG },
        { pad_button_number(HidNpadButton_X), X_FLAG },
        { pad_button_number(HidNpadButton_Y), Y_FLAG },
        { pad_button_number(HidNpadButton_StickL), LS_CLK_FLAG },
        { pad_button_number(HidNpadButton_StickR), RS_CLK_FLAG },
        { pad_button_number(HidNpadButton_L), LB_FLAG },
        { pad_button_number(HidNpadButton_R), RB_FLAG },
        { pad_button_number(HidNpadButton_Plus), PLAY_FLAG },
        { pad_button_number(HidNpadButton_Minus), BACK_FLAG },
        { pad_button_number(HidNpadButton_Up), UP_FLAG },
        { pad_button_number(HidNpadButton_Down), DOWN_FLAG },
        { pad_button_number(HidNpadButton_Left), LEFT_FLAG },
        { pad_button_number(HidNpadButton_Right), RIGHT_FLAG }
    };
    return map[button];
}

static inline void padUpdateOverKeyboard(PadState* pad) {
    static int glfw_keyboard_state[GLFW_KEY_LAST] = {0};
    
    static std::map<int, HidNpadButton> keyboard_map = {
        { GLFW_KEY_UP, HidNpadButton_Up },
        { GLFW_KEY_DOWN, HidNpadButton_Down },
        { GLFW_KEY_LEFT, HidNpadButton_Left },
        { GLFW_KEY_RIGHT, HidNpadButton_Right },
        { GLFW_KEY_A, HidNpadButton_A },
        { GLFW_KEY_B, HidNpadButton_B },
        { GLFW_KEY_X, HidNpadButton_X },
        { GLFW_KEY_Y, HidNpadButton_Y },
        { GLFW_KEY_Q, HidNpadButton_L },
        { GLFW_KEY_E, HidNpadButton_R },
        { GLFW_KEY_Z, HidNpadButton_ZL },
        { GLFW_KEY_C, HidNpadButton_ZR },
        { GLFW_KEY_MINUS, HidNpadButton_Minus },
        { GLFW_KEY_EQUAL, HidNpadButton_Plus },
        { GLFW_KEY_N, HidNpadButton_StickL },
        { GLFW_KEY_M, HidNpadButton_StickR }
    };
    
    static std::map<int, std::vector<int>> axis_map = {
        { GLFW_KEY_T, { 0, 1, 1 } },
        { GLFW_KEY_G, { 0, 1, -1 } },
        { GLFW_KEY_F, { 0, 0, -1 } },
        { GLFW_KEY_H, { 0, 0, 1 } },
        { GLFW_KEY_I, { 1, 1, 1 } },
        { GLFW_KEY_K, { 1, 1, -1 } },
        { GLFW_KEY_J, { 1, 0, -1 } },
        { GLFW_KEY_L, { 1, 0, 1 } }
    };
    
    for (auto &pair: keyboard_map) {
        int is_pressed = glfwGetKey(glfwGetCurrentContext(), pair.first);
        
        if (glfw_keyboard_state[pair.first] != is_pressed) {
            glfw_keyboard_state[pair.first] = is_pressed;
            
            if (is_pressed) {
                pad->buttons_cur |= pair.second;
            } else {
                pad->buttons_cur &= ~pair.second;
            }
        }
    }
    
    for (auto &pair: axis_map) {
        int is_pressed = glfwGetKey(glfwGetCurrentContext(), pair.first);
        
        if (glfw_keyboard_state[pair.first] != is_pressed) {
            glfw_keyboard_state[pair.first] = is_pressed;
            
            if (pair.second[1] == 0) {
                pad->sticks[pair.second[0]].x = is_pressed ? pair.second[2] * 0x7FFF : 0;
            } else {
                pad->sticks[pair.second[0]].y = is_pressed ? pair.second[2] * 0x7FFF : 0;
            }
        }
    }
}

GamepadFrontendSwitch::GamepadFrontendSwitch() {
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&m_pad_state);
    
    hidInitializeVibrationDevices(m_vibration_device_handles[0], 2, HidNpadIdType_Handheld, HidNpadStyleTag_NpadHandheld);
    hidInitializeVibrationDevices(m_vibration_device_handles[1], 2, HidNpadIdType_No1, HidNpadStyleTag_NpadJoyDual);
}

void GamepadFrontendSwitch::handle_gamepad() {
    PadState state = m_pad_state;
    
#ifdef __SWITCH__
    padUpdate(&state);
#else
    padUpdateOverKeyboard(&state);
#endif
    
    if (set_new_pad_state(state)) {
        u64 buttons = padGetButtons(&m_pad_state);
        
        for (int i = 0; i < 28; i++) {
            bool is_pressed = buttons & BIT(i);
            
            if (is_pressed) {
                m_gamepad_state.buttonFlags |= pad_button_to_moonlight_button(i);
            } else {
                m_gamepad_state.buttonFlags &= ~pad_button_to_moonlight_button(i);
            }
        }
        
        m_gamepad_state.leftTrigger = (buttons & HidNpadButton_ZL) ? 0xFF : 0;
        m_gamepad_state.rightTrigger = (buttons & HidNpadButton_ZR) ? 0xFF : 0;
        m_gamepad_state.leftStickX = m_pad_state.sticks[0].x;
        m_gamepad_state.leftStickY = m_pad_state.sticks[0].y;
        m_gamepad_state.rightStickX = m_pad_state.sticks[1].x;
        m_gamepad_state.rightStickY = m_pad_state.sticks[1].y;
    }
}

void GamepadFrontendSwitch::handle_rumple(unsigned short controller, unsigned short low_freq_motor, unsigned short high_freq_motor) {
    if (controller == 0) {
        float low = (float)low_freq_motor / 0xFFFF;
        float high = (float)high_freq_motor / 0xFFFF;
        
        memset(m_vibration_values, 0, sizeof(m_vibration_values));
        
        m_vibration_values[0].amp_low   = low;
        m_vibration_values[0].freq_low  = low * 50;
        m_vibration_values[0].amp_high  = high;
        m_vibration_values[0].freq_high = high * 100;
        
        m_vibration_values[1].amp_low   = low;
        m_vibration_values[1].freq_low  = low * 50;
        m_vibration_values[1].amp_high  = high;
        m_vibration_values[1].freq_high = high * 100;
        
        int target_device = padIsHandheld(&m_pad_state) ? 0 : 1;
        hidSendVibrationValues(m_vibration_device_handles[target_device], m_vibration_values, 2);
    }
}

void GamepadFrontendSwitch::stop_rumple() {
    HidVibrationValue stop;
    memset(&stop, 0, sizeof(HidVibrationValue));
    
    stop.freq_low  = 160.0f;
    stop.freq_high = 320.0f;
    
    memcpy(&m_vibration_values[0], &stop, sizeof(HidVibrationValue));
    memcpy(&m_vibration_values[1], &stop, sizeof(HidVibrationValue));
    
    int target_device = padIsHandheld(&m_pad_state) ? 0 : 1;
    hidSendVibrationValues(m_vibration_device_handles[target_device], m_vibration_values, 2);
    hidSendVibrationValues(m_vibration_device_handles[1 - target_device], m_vibration_values, 2);
}

bool GamepadFrontendSwitch::set_new_pad_state(PadState& state) {
    if (!pad_is_equal(m_pad_state, state)) {
        m_pad_state = state;
        return true;
    }
    return false;
}
