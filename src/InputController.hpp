#include <stdio.h>
#include <stdint.h>
#pragma once

struct GLFWgamepadstate;

struct GamePadState {
    short buttonFlags;
    unsigned char leftTrigger;
    unsigned char rightTrigger;
    short leftStickX;
    short leftStickY;
    short rightStickX;
    short rightStickY;
};

extern struct GamePadState game_pad_state;

#define GAME_PAD_COMBO(KEY) \
    ((game_pad_state.buttonFlags & LB_FLAG) && (game_pad_state.buttonFlags & RB_FLAG) && (game_pad_state.buttonFlags & KEY))

#define GAME_PAD_COMBO_R(KEY) \
    (game_pad_state.leftTrigger && game_pad_state.rightTrigger && (game_pad_state.buttonFlags & KEY))

class InputController {
public:
    static InputController* controller() {
        static InputController controller;
        return &controller;
    }
    
    void handle_cursor_event(int width, int height, int x, int y);
    void handle_mouse_event(int button, int action, int modifiers);
    void handle_keyboard_event(int key, int scancode, int action, int modifiers);
    void handle_gamepad_event(GLFWgamepadstate* gamepad);
    
    void send_to_stream();
    
private:
    InputController() {};
};
