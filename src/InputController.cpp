#include "InputController.hpp"
#include "Settings.hpp"
#include "Limelight.h"
#include "libretro.h"
#include "Limelight.h"
#include <nanogui/nanogui.h>
#include <nanogui/opengl.h>

extern retro_input_state_t input_state_cb;

int16_t keyboard_state[RETROK_LAST];
struct MouseState mouse_state;
struct GamePadState game_pad_state;

static int m_width = 1, m_height = 1;

void InputController::handle_mouse(int width, int height) {
    m_width = width;
    m_height = height;
    
    int mouse_l_press = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_LEFT);
    int mouse_r_press = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_RIGHT);
    
    int mouse_x = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_X);
    int mouse_y = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_Y);
    int pointer_x = 0, pointer_y = 0;
    
    // TODO: Pointer on macOS currently broken
    // https://github.com/libretro/RetroArch/issues/10168
    if (input_state_cb(0, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_PRESSED)) {
        pointer_x = (input_state_cb(0, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_X) + 0x7fff) * width / 0xffff;
        pointer_y = (input_state_cb(0, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_Y) + 0x7fff) * height / 0xffff;
    }
    
    #ifndef __LAKKA_SWITCH__
    if (mouse_x != 0 && mouse_y != 0) {
        mouse_state.x = mouse_x;
        mouse_state.y = mouse_y;
        nanogui::cursor_pos_callback_event(mouse_x, mouse_y);
    } else
    #endif
    if (mouse_l_press && pointer_x != 0 && pointer_y != 0) {
        mouse_state.x = pointer_x;
        mouse_state.y = pointer_y;
        nanogui::cursor_pos_callback_event(pointer_x, pointer_y);
    }
    
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

void InputController::handle_keyboard() {
    //
}

void InputController::handle_game_pad() {
    game_pad_state.leftTrigger = 0xFFFF * input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L2);
    game_pad_state.rightTrigger = 0xFFFF * input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R2);
    
    game_pad_state.leftStickX = input_state_cb(0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_X);
    game_pad_state.leftStickY = 0xFFFF - input_state_cb(0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_Y);
    game_pad_state.rightStickX = input_state_cb(0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_X);
    game_pad_state.rightStickY = 0xFFFF - input_state_cb(0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_Y);
    
    #define SET_GAME_PAD_STATE(LIMELIGHT_KEY, RETRO_KEY) \
        input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_KEY) ? (game_pad_state.buttonFlags |= LIMELIGHT_KEY) : (game_pad_state.buttonFlags &= ~LIMELIGHT_KEY);
    
    SET_GAME_PAD_STATE(UP_FLAG, RETRO_DEVICE_ID_JOYPAD_UP);
    SET_GAME_PAD_STATE(DOWN_FLAG, RETRO_DEVICE_ID_JOYPAD_DOWN);
    SET_GAME_PAD_STATE(LEFT_FLAG, RETRO_DEVICE_ID_JOYPAD_LEFT);
    SET_GAME_PAD_STATE(RIGHT_FLAG, RETRO_DEVICE_ID_JOYPAD_RIGHT);
    
    SET_GAME_PAD_STATE(BACK_FLAG, RETRO_DEVICE_ID_JOYPAD_SELECT);
    SET_GAME_PAD_STATE(PLAY_FLAG, RETRO_DEVICE_ID_JOYPAD_START);
    
    SET_GAME_PAD_STATE(LB_FLAG, RETRO_DEVICE_ID_JOYPAD_L);
    SET_GAME_PAD_STATE(RB_FLAG, RETRO_DEVICE_ID_JOYPAD_R);
    
    SET_GAME_PAD_STATE(LS_CLK_FLAG, RETRO_DEVICE_ID_JOYPAD_L3);
    SET_GAME_PAD_STATE(RS_CLK_FLAG, RETRO_DEVICE_ID_JOYPAD_R3);
    
    if (Settings::settings()->swap_ab_xy()) {
        SET_GAME_PAD_STATE(A_FLAG, RETRO_DEVICE_ID_JOYPAD_B);
        SET_GAME_PAD_STATE(B_FLAG, RETRO_DEVICE_ID_JOYPAD_A);
        SET_GAME_PAD_STATE(X_FLAG, RETRO_DEVICE_ID_JOYPAD_Y);
        SET_GAME_PAD_STATE(Y_FLAG, RETRO_DEVICE_ID_JOYPAD_X);
    } else {
        SET_GAME_PAD_STATE(A_FLAG, RETRO_DEVICE_ID_JOYPAD_A);
        SET_GAME_PAD_STATE(B_FLAG, RETRO_DEVICE_ID_JOYPAD_B);
        SET_GAME_PAD_STATE(X_FLAG, RETRO_DEVICE_ID_JOYPAD_X);
        SET_GAME_PAD_STATE(Y_FLAG, RETRO_DEVICE_ID_JOYPAD_Y);
    }
}

void InputController::send_to_stream() {
#if defined(__LAKKA_SWITCH__)
    static int mouse_x = mouse_state.x, mouse_y = mouse_state.y;
    static int last_mouse_x = mouse_state.x, last_mouse_y = mouse_state.y;
    
    bool move_mouse = (game_pad_state.leftTrigger == 0) && (game_pad_state.rightTrigger == 0);
    
    if (move_mouse) {
        mouse_x = std::min(std::max(mouse_x + mouse_state.x - last_mouse_x, 0), m_width);
        mouse_y = std::min(std::max(mouse_y + mouse_state.y - last_mouse_y, 0), m_height);
        
        LiSendMousePositionEvent(mouse_x, mouse_y, m_width, m_height);
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
