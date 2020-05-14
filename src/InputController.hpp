#include <stdio.h>
#include <stdint.h>
#pragma once

extern int16_t keyboard_state[];

struct MouseState {
    int x;
    int y;
    bool l_press;
    bool r_press;
};

extern struct MouseState mouse_state;

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
    
    void handle_input(int width, int height) {
        handle_mouse(width, height);
        handle_keyboard();
        handle_game_pad();
    };
    
    void send_to_stream();
    
private:
    InputController() {};
    
    void handle_mouse(int width, int height);
    void handle_keyboard();
    void handle_game_pad();
};
