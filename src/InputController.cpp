#include "InputController.hpp"
#include "Settings.hpp"
#include "Limelight.h"
#include "Limelight.h"
#include <nanogui/nanogui.h>
#include <nanogui/opengl.h>
#include <GLFW/glfw3.h>

#ifdef __SWITCH__
#include <switch.h>
static u32 VibrationDeviceHandles[2][2];
static HidVibrationValue VibrationValues[2];
#endif

struct MouseState {
    int x;
    int y;
    bool l_press;
    bool r_press;
};

struct MouseState mouse_state;
struct GamePadState game_pad_state;

static int m_width = 1, m_height = 1;

InputController::InputController() {
    #ifdef __SWITCH__
    hidInitializeVibrationDevices(VibrationDeviceHandles[0], 2, CONTROLLER_HANDHELD, TYPE_HANDHELD);
    hidInitializeVibrationDevices(VibrationDeviceHandles[1], 2, CONTROLLER_PLAYER_1, TYPE_JOYCON_PAIR);
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
    bool mouse_l_press = button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS;
    bool mouse_r_press = button == GLFW_MOUSE_BUTTON_2 && action == GLFW_PRESS;
    
    if (!mouse_state.l_press && mouse_l_press) {
        mouse_state.l_press = true;
        nanogui::mouse_button_callback_event(NANOGUI_MOUSE_BUTTON_1, NANOGUI_PRESS, 0);
    } else if (mouse_state.l_press && !mouse_l_press) {
        mouse_state.l_press = false;
        nanogui::mouse_button_callback_event(NANOGUI_MOUSE_BUTTON_1, NANOGUI_RELEASE, 0);
    }
    
    if (!mouse_state.r_press && mouse_r_press) {
        mouse_state.r_press = true;
        nanogui::mouse_button_callback_event(NANOGUI_MOUSE_BUTTON_2, NANOGUI_PRESS, 0);
    } else if (mouse_state.r_press && !mouse_r_press) {
        mouse_state.r_press = false;
        nanogui::mouse_button_callback_event(NANOGUI_MOUSE_BUTTON_2, NANOGUI_RELEASE, 0);
    }
}

void InputController::handle_keyboard_event(int key, int scancode, int action, int modifiers) {
    nanogui::keyboard_event(key, scancode, action, modifiers);
    
    #ifndef __SWITCH__
    // Translate keyboard keys to gamepad
    extern GLFWgamepadstate glfw_gamepad_state;
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
    
    glfw_gamepad_state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] = glfw_keyboard_state[GLFW_KEY_Z] ? 1 : -1;
    glfw_gamepad_state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] = glfw_keyboard_state[GLFW_KEY_C] ? 1 : -1;
    
    handle_gamepad_event(&glfw_gamepad_state);
    #endif
}

void InputController::handle_gamepad_event(GLFWgamepadstate* gamepad) {
    game_pad_state.leftTrigger = 0xFFFF * (gamepad->axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] + 1) / 2;
    game_pad_state.rightTrigger = 0xFFFF * (gamepad->axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] + 1) / 2;
    game_pad_state.leftStickX = gamepad->axes[GLFW_GAMEPAD_AXIS_LEFT_X] * 0x7FFF;
    game_pad_state.leftStickY = 0xFFFF - gamepad->axes[GLFW_GAMEPAD_AXIS_LEFT_Y] * 0x7FFF;
    game_pad_state.rightStickX = gamepad->axes[GLFW_GAMEPAD_AXIS_RIGHT_X] * 0x7FFF;
    game_pad_state.rightStickY = 0xFFFF - gamepad->axes[GLFW_GAMEPAD_AXIS_RIGHT_Y] * 0x7FFF;
    
    #define SET_GAME_PAD_STATE(LIMELIGHT_KEY, GLFW_GAMEPAD_BUTTON) \
        gamepad->buttons[GLFW_GAMEPAD_BUTTON] ? (game_pad_state.buttonFlags |= LIMELIGHT_KEY) : (game_pad_state.buttonFlags &= ~LIMELIGHT_KEY);
    
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
    
    if (Settings::settings()->swap_ab_xy()) {
        SET_GAME_PAD_STATE(A_FLAG, GLFW_GAMEPAD_BUTTON_B);
        SET_GAME_PAD_STATE(B_FLAG, GLFW_GAMEPAD_BUTTON_A);
        SET_GAME_PAD_STATE(X_FLAG, GLFW_GAMEPAD_BUTTON_Y);
        SET_GAME_PAD_STATE(Y_FLAG, GLFW_GAMEPAD_BUTTON_X);
    } else {
        SET_GAME_PAD_STATE(A_FLAG, GLFW_GAMEPAD_BUTTON_A);
        SET_GAME_PAD_STATE(B_FLAG, GLFW_GAMEPAD_BUTTON_B);
        SET_GAME_PAD_STATE(X_FLAG, GLFW_GAMEPAD_BUTTON_X);
        SET_GAME_PAD_STATE(Y_FLAG, GLFW_GAMEPAD_BUTTON_Y);
    }
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

void InputController::send_to_stream() {
#if defined(__SWITCH__)
    static bool pressed = false;
    static int mouse_x = mouse_state.x, mouse_y = mouse_state.y;
    static int last_mouse_x = 0, last_mouse_y = 0;
    
    if (mouse_state.l_press && !pressed) {
        pressed = true;
        last_mouse_x = mouse_state.x;
        last_mouse_y = mouse_state.y;
    } else if (!mouse_state.l_press && pressed) {
        pressed = false;
    }
    
    bool move_mouse = (game_pad_state.leftTrigger == 0) && (game_pad_state.rightTrigger == 0);
    
    if (move_mouse) {
        static int last_send_mouse_x = 0, last_send_mouse_y = 0;
        
        mouse_x = std::min(std::max(mouse_x + mouse_state.x - last_mouse_x, 0), m_width);
        mouse_y = std::min(std::max(mouse_y + mouse_state.y - last_mouse_y, 0), m_height);
        
        if (mouse_x != last_send_mouse_x || mouse_y != last_send_mouse_y) {
            LiSendMousePositionEvent(mouse_x, mouse_y, m_width, m_height);
            last_send_mouse_x = mouse_x;
            last_send_mouse_y = mouse_y;
        }
    }
    
    last_mouse_x = mouse_state.x;
    last_mouse_y = mouse_state.y;
    
    if (mouse_state.l_press && ((game_pad_state.buttonFlags & LB_FLAG) || game_pad_state.leftTrigger)) {
        LiSendMouseButtonEvent(BUTTON_ACTION_PRESS, BUTTON_LEFT);
    } else {
        LiSendMouseButtonEvent(BUTTON_ACTION_RELEASE, BUTTON_LEFT);
    }
    
    if (mouse_state.l_press && ((game_pad_state.buttonFlags & RB_FLAG) || game_pad_state.rightTrigger)) {
        LiSendMouseButtonEvent(BUTTON_ACTION_PRESS, BUTTON_RIGHT);
    } else {
        LiSendMouseButtonEvent(BUTTON_ACTION_RELEASE, BUTTON_RIGHT);
    }
#else
    LiSendMousePositionEvent(mouse_state.x, mouse_state.y, m_width, m_height);
    LiSendMouseButtonEvent(mouse_state.l_press ? BUTTON_ACTION_PRESS : BUTTON_ACTION_RELEASE, BUTTON_LEFT);
    LiSendMouseButtonEvent(mouse_state.r_press ? BUTTON_ACTION_PRESS : BUTTON_ACTION_RELEASE, BUTTON_RIGHT);
#endif
    
    static bool send_alt_enter = false;
    
    if (!send_alt_enter && GAME_PAD_COMBO(LEFT_FLAG)) {
        send_alt_enter = true;
        LiSendKeyboardEvent(0x0D, KEY_ACTION_DOWN, MODIFIER_ALT);
    } else if (send_alt_enter && !GAME_PAD_COMBO(LEFT_FLAG)) {
        send_alt_enter = false;
        LiSendKeyboardEvent(0x0D, KEY_ACTION_UP, MODIFIER_ALT);
    }
    
    static bool send_escape = false;
    
    if (!send_escape && GAME_PAD_COMBO(RIGHT_FLAG)) {
        send_escape = true;
        LiSendKeyboardEvent(0x1B, KEY_ACTION_DOWN, 0);
    } else if (send_escape && !GAME_PAD_COMBO(RIGHT_FLAG)) {
        send_escape = false;
        LiSendKeyboardEvent(0x1B, KEY_ACTION_UP, 0);
    }
    
    LiSendControllerEvent(
        game_pad_state.buttonFlags,
        game_pad_state.leftTrigger,
        game_pad_state.rightTrigger,
        game_pad_state.leftStickX,
        game_pad_state.leftStickY,
        game_pad_state.rightStickX,
        game_pad_state.rightStickY
    );
}
