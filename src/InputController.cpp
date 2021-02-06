#include "InputController.hpp"
#include "Settings.hpp"
#include "Limelight.h"
#include <nanogui/nanogui.h>
#include <nanogui/opengl.h>
#include <GLFW/glfw3.h>

#ifdef __SWITCH__
#include <switch.h>
static HidVibrationDeviceHandle VibrationDeviceHandles[2][2];
static HidVibrationValue VibrationValues[2];
static HidVibrationValue VibrationValue_stop;
#endif

struct MouseState {
    int x;
    int y;
    bool is_pressed;
};

struct MouseState mouse_state;
static GLFWgamepadstate glfw_gamepad_state;

static int m_width = 1, m_height = 1;
static double m_scroll_x = 0, m_scroll_y = 0;

InputController::InputController() {
    #ifdef __SWITCH__
    hidInitializeVibrationDevices(VibrationDeviceHandles[0], 2, HidNpadIdType_Handheld, HidNpadStyleTag_NpadHandheld);
    hidInitializeVibrationDevices(VibrationDeviceHandles[1], 2, HidNpadIdType_No1, HidNpadStyleTag_NpadJoyDual);
    
    VibrationValue_stop.freq_low  = 160.0f;
    VibrationValue_stop.freq_high = 320.0f;
    #endif
}

void InputController::handle_cursor_event(int width, int height, int x, int y) {
    m_width = width;
    m_height = height;
    mouse_state.x = x;
    mouse_state.y = y;
    nanogui::cursor_pos_callback_event(x, y);
}

void InputController::handle_mouse_event(int button, int action, int modifiers) {
    bool is_pressed = button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS;
    
    if (!mouse_state.is_pressed && is_pressed) {
        mouse_state.is_pressed = true;
        nanogui::mouse_button_callback_event(NANOGUI_MOUSE_BUTTON_1, NANOGUI_PRESS, 0);
    } else if (mouse_state.is_pressed && !is_pressed) {
        mouse_state.is_pressed = false;
        nanogui::mouse_button_callback_event(NANOGUI_MOUSE_BUTTON_1, NANOGUI_RELEASE, 0);
    }
}

void InputController::handle_scroll(double x, double y) {
    m_scroll_x = x;
    m_scroll_y = y;
    nanogui::scroll_callback_event(x, y);
}

void InputController::handle_keyboard_event(int key, int scancode, int action, int modifiers) {
    nanogui::keyboard_callback_event(key, scancode, action, modifiers);
    
    #ifndef __SWITCH__
    // Translate keyboard keys to gamepad
    GLFWgamepadstate glfw_gamepad_state;
    memset(&glfw_gamepad_state, 0, sizeof(GLFWgamepadstate));
    static int glfw_keyboard_state[GLFW_KEY_LAST];
    glfw_keyboard_state[key] = action != GLFW_RELEASE;
    
    #define GLFW_KEYBOARD_TO_GAMEPAD(GLFW_KEY, GLFW_GAMEPAD_BUTTON) \
        glfw_gamepad_state.buttons[GLFW_GAMEPAD_BUTTON] = glfw_keyboard_state[GLFW_KEY]
    
    GLFW_KEYBOARD_TO_GAMEPAD(GLFW_KEY_UP, GLFW_GAMEPAD_BUTTON_DPAD_UP);
    GLFW_KEYBOARD_TO_GAMEPAD(GLFW_KEY_DOWN, GLFW_GAMEPAD_BUTTON_DPAD_DOWN);
    GLFW_KEYBOARD_TO_GAMEPAD(GLFW_KEY_LEFT, GLFW_GAMEPAD_BUTTON_DPAD_LEFT);
    GLFW_KEYBOARD_TO_GAMEPAD(GLFW_KEY_RIGHT, GLFW_GAMEPAD_BUTTON_DPAD_RIGHT);
    GLFW_KEYBOARD_TO_GAMEPAD(GLFW_KEY_Q, GLFW_GAMEPAD_BUTTON_LEFT_BUMPER);
    GLFW_KEYBOARD_TO_GAMEPAD(GLFW_KEY_E, GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER);
    GLFW_KEYBOARD_TO_GAMEPAD(GLFW_KEY_A, GLFW_GAMEPAD_BUTTON_A);
    GLFW_KEYBOARD_TO_GAMEPAD(GLFW_KEY_B, GLFW_GAMEPAD_BUTTON_B);
    GLFW_KEYBOARD_TO_GAMEPAD(GLFW_KEY_X, GLFW_GAMEPAD_BUTTON_X);
    GLFW_KEYBOARD_TO_GAMEPAD(GLFW_KEY_Y, GLFW_GAMEPAD_BUTTON_Y);
    GLFW_KEYBOARD_TO_GAMEPAD(GLFW_KEY_ESCAPE, GLFW_GAMEPAD_BUTTON_START);
    GLFW_KEYBOARD_TO_GAMEPAD(GLFW_KEY_MINUS, GLFW_GAMEPAD_BUTTON_BACK);
    GLFW_KEYBOARD_TO_GAMEPAD(GLFW_KEY_R, GLFW_GAMEPAD_BUTTON_LEFT_THUMB);
    GLFW_KEYBOARD_TO_GAMEPAD(GLFW_KEY_T, GLFW_GAMEPAD_BUTTON_RIGHT_THUMB);
    
    glfw_gamepad_state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] = glfw_keyboard_state[GLFW_KEY_Z] ? 1 : -1;
    glfw_gamepad_state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] = glfw_keyboard_state[GLFW_KEY_C] ? 1 : -1;
    
    handle_gamepad_event(glfw_gamepad_state);
    #endif
}

void InputController::handle_gamepad_event(GLFWgamepadstate& gamepad) {
    if (gamepad.buttons != glfw_gamepad_state.buttons) {
        for (int j = 0; j <= GLFW_GAMEPAD_BUTTON_LAST; j++) {
            if (gamepad.buttons[j] == glfw_gamepad_state.buttons[j]) {
                continue;
            }
            
            nanogui::gamepad_button_callback_event(0, j, gamepad.buttons[j]);
        }
    }
    
    if (gamepad.axes != glfw_gamepad_state.axes) {
        for (int j = 0; j <= GLFW_GAMEPAD_AXIS_LAST; j++) {
            if (gamepad.axes[j] == glfw_gamepad_state.axes[j]) {
                continue;
            }
            
            nanogui::gamepad_analog_callback_event(0, j, gamepad.axes[j]);
        }
    }
    
    glfw_gamepad_state = gamepad;
}

void InputController::handle_rumple(unsigned short low_freq_motor, unsigned short high_freq_motor) {
    #ifdef __SWITCH__
    float low = (float)low_freq_motor / 0xFFFF;
    float high = (float)high_freq_motor / 0xFFFF;
    
    VibrationValues[0].amp_low   = low;
    VibrationValues[0].freq_low  = low * 50;
    VibrationValues[0].amp_high  = high;
    VibrationValues[0].freq_high = high * 50;
    
    VibrationValues[1].amp_low   = low;
    VibrationValues[1].freq_low  = low * 50;
    VibrationValues[1].amp_high  = high;
    VibrationValues[1].freq_high = high * 50;
    
    int target_device = 0;
    if (!hidGetHandheldMode())
        target_device = 1;
    
    hidSendVibrationValues(VibrationDeviceHandles[target_device], VibrationValues, 2);
    #endif
}

void InputController::stop_rumple() {
    #ifdef __SWITCH__
    memcpy(&VibrationValues[0], &VibrationValue_stop, sizeof(HidVibrationValue));
    memcpy(&VibrationValues[1], &VibrationValue_stop, sizeof(HidVibrationValue));
    
    int target_device = 0;
    if (!hidGetHandheldMode())
        target_device = 1;
    
    hidSendVibrationValues(VibrationDeviceHandles[target_device], VibrationValues, 2);
    #endif
}

bool InputController::gamepad_combo_is_enabled(GamepadCombo combo) {
    return GamepadMapper::mapper()->gamepad_combo_is_enabled(glfw_gamepad_state, combo);
}

bool InputController::gamepad_button_is_enabled(int button) {
    return glfw_gamepad_state.buttons[button];
}

bool InputController::gamepad_trigger_is_enabled(int trigger) {
    return glfw_gamepad_state.axes[trigger] > 0;
}

void InputController::send_to_stream() {
    // Mouse
    static bool is_pressed = false, is_released = false;
    static int last_mouse_x = 0, last_mouse_y = 0;
    static int last_send_mouse_x = 0, last_send_mouse_y = 0;
    
    if (mouse_state.is_pressed && !is_pressed) {
        is_pressed = true;
        last_mouse_x = mouse_state.x;
        last_mouse_y = mouse_state.y;
    } else if (!mouse_state.is_pressed && is_pressed) {
        is_pressed = false;
    }
    
    if (Settings::settings()->click_by_tap()) {
        if (last_send_mouse_x != mouse_state.x || last_send_mouse_y != mouse_state.y) {
            LiSendMousePositionEvent(mouse_state.x, mouse_state.y, m_width, m_height);
            last_send_mouse_x = mouse_state.x;
            last_send_mouse_y = mouse_state.y;
        }
        
        if (gamepad_button_is_enabled(NANOGUI_GAMEPAD_BUTTON_RIGHT_BUMPER) || gamepad_trigger_is_enabled(NANOGUI_GAMEPAD_AXIS_RIGHT_TRIGGER)) {
            if (mouse_state.is_pressed) {
                is_released = false;
                LiSendMouseButtonEvent(BUTTON_ACTION_PRESS, BUTTON_RIGHT);
            } else if (!is_released) {
                is_released = true;
                LiSendMouseButtonEvent(BUTTON_ACTION_RELEASE, BUTTON_RIGHT);
            }
        } else {
            if (mouse_state.is_pressed) {
                is_released = false;
                LiSendMouseButtonEvent(BUTTON_ACTION_PRESS, BUTTON_LEFT);
            } else if (!is_released) {
                is_released = true;
                LiSendMouseButtonEvent(BUTTON_ACTION_RELEASE, BUTTON_LEFT);
            }
        }
    } else {
        bool move_mouse = !gamepad_trigger_is_enabled(NANOGUI_GAMEPAD_AXIS_LEFT_TRIGGER) && !gamepad_trigger_is_enabled(NANOGUI_GAMEPAD_AXIS_RIGHT_TRIGGER);
        
        if (move_mouse) {
            int relative_mouse_x = mouse_state.x - last_mouse_x;
            int relative_mouse_y = mouse_state.y - last_mouse_y;
            
            if (relative_mouse_x != last_send_mouse_x || relative_mouse_y != last_send_mouse_y) {
                LiSendMouseMoveEvent(relative_mouse_x, relative_mouse_y);
                last_send_mouse_x = relative_mouse_x;
                last_send_mouse_y = relative_mouse_y;
            }
        }
        
        last_mouse_x = mouse_state.x;
        last_mouse_y = mouse_state.y;
        
        if (gamepad_button_is_enabled(NANOGUI_GAMEPAD_BUTTON_LEFT_BUMPER) || gamepad_trigger_is_enabled(NANOGUI_GAMEPAD_AXIS_LEFT_TRIGGER)) {
            if (mouse_state.is_pressed) {
                is_released = false;
                LiSendMouseButtonEvent(BUTTON_ACTION_PRESS, BUTTON_LEFT);
            } else if (!is_released) {
                is_released = true;
                LiSendMouseButtonEvent(BUTTON_ACTION_RELEASE, BUTTON_LEFT);
            }
        } else if (gamepad_button_is_enabled(NANOGUI_GAMEPAD_BUTTON_RIGHT_BUMPER) || gamepad_trigger_is_enabled(NANOGUI_GAMEPAD_AXIS_RIGHT_TRIGGER)) {
            if (mouse_state.is_pressed) {
                is_released = false;
                LiSendMouseButtonEvent(BUTTON_ACTION_PRESS, BUTTON_RIGHT);
            } else if (!is_released) {
                is_released = true;
                LiSendMouseButtonEvent(BUTTON_ACTION_RELEASE, BUTTON_RIGHT);
            }
        }
    }
    
    // Scroll
    if (m_scroll_y != 0) {
        LiSendHighResScrollEvent(m_scroll_y > 0 ? fmax(m_scroll_y, 1) : fmin(m_scroll_y, -1));
        m_scroll_y = 0;
    }
    
    // Keyboard
    static bool send_alt_enter = false;
    
    if (!send_alt_enter && gamepad_combo_is_enabled(GamepadComboAltEnter)) {
        send_alt_enter = true;
        LiSendKeyboardEvent(0x0D, KEY_ACTION_DOWN, MODIFIER_ALT);
    } else if (send_alt_enter && !gamepad_combo_is_enabled(GamepadComboAltEnter)) {
        send_alt_enter = false;
        LiSendKeyboardEvent(0x0D, KEY_ACTION_UP, MODIFIER_ALT);
    }
    
    static bool send_escape = false;
    
    if (!send_escape && gamepad_combo_is_enabled(GamepadComboEscape)) {
        send_escape = true;
        LiSendKeyboardEvent(0x1B, KEY_ACTION_DOWN, 0);
    } else if (send_escape && !gamepad_combo_is_enabled(GamepadComboEscape)) {
        send_escape = false;
        LiSendKeyboardEvent(0x1B, KEY_ACTION_UP, 0);
    }
    
    // Gamepad
    auto mapped_gamepad = GamepadMapper::mapper()->map(glfw_gamepad_state);
    short buttonFlags = 0;
    unsigned char leftTrigger = 0xFFFF * (mapped_gamepad.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] + 1) / 2;
    unsigned char rightTrigger = 0xFFFF * (mapped_gamepad.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] + 1) / 2;
    short leftStickX = mapped_gamepad.axes[GLFW_GAMEPAD_AXIS_LEFT_X] * 0x7FFF;
    short leftStickY = 0xFFFF - mapped_gamepad.axes[GLFW_GAMEPAD_AXIS_LEFT_Y] * 0x7FFF;
    short rightStickX = mapped_gamepad.axes[GLFW_GAMEPAD_AXIS_RIGHT_X] * 0x7FFF;
    short rightStickY = 0xFFFF - mapped_gamepad.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y] * 0x7FFF;
    
    if (GamepadMapper::mapper()->gamepad_combo_is_enabled(glfw_gamepad_state, GamepadComboGuide)) {
        buttonFlags |= SPECIAL_FLAG;
    }
    
    #define SET_GAME_PAD_STATE(LIMELIGHT_KEY, GLFW_GAMEPAD_BUTTON) \
        mapped_gamepad.buttons[GLFW_GAMEPAD_BUTTON] ? (buttonFlags |= LIMELIGHT_KEY) : (buttonFlags &= ~LIMELIGHT_KEY);
    
    SET_GAME_PAD_STATE(UP_FLAG, GLFW_GAMEPAD_BUTTON_DPAD_UP);
    SET_GAME_PAD_STATE(DOWN_FLAG, GLFW_GAMEPAD_BUTTON_DPAD_DOWN);
    SET_GAME_PAD_STATE(LEFT_FLAG, GLFW_GAMEPAD_BUTTON_DPAD_LEFT);
    SET_GAME_PAD_STATE(RIGHT_FLAG, GLFW_GAMEPAD_BUTTON_DPAD_RIGHT);
    
    SET_GAME_PAD_STATE(BACK_FLAG, GLFW_GAMEPAD_BUTTON_BACK);
    SET_GAME_PAD_STATE(PLAY_FLAG, GLFW_GAMEPAD_BUTTON_START);
    
    SET_GAME_PAD_STATE(LB_FLAG, GLFW_GAMEPAD_BUTTON_LEFT_BUMPER);
    SET_GAME_PAD_STATE(RB_FLAG, GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER);
    
    SET_GAME_PAD_STATE(LS_CLK_FLAG, GLFW_GAMEPAD_BUTTON_LEFT_THUMB);
    SET_GAME_PAD_STATE(RS_CLK_FLAG, GLFW_GAMEPAD_BUTTON_RIGHT_THUMB);
    
    SET_GAME_PAD_STATE(A_FLAG, GLFW_GAMEPAD_BUTTON_A);
    SET_GAME_PAD_STATE(B_FLAG, GLFW_GAMEPAD_BUTTON_B);
    SET_GAME_PAD_STATE(X_FLAG, GLFW_GAMEPAD_BUTTON_X);
    SET_GAME_PAD_STATE(Y_FLAG, GLFW_GAMEPAD_BUTTON_Y);
    
    LiSendControllerEvent(
        buttonFlags,
        leftTrigger,
        rightTrigger,
        leftStickX,
        leftStickY,
        rightStickX,
        rightStickY
    );
}
