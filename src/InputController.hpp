#include "GamepadMapper.hpp"
#pragma once

struct GLFWgamepadstate;

class InputController {
public:
    static InputController* controller() {
        static InputController controller;
        return &controller;
    }
    
    void handle_cursor_event(int width, int height, int x, int y);
    void handle_mouse_event(int button, int action, int modifiers);
    void handle_scroll(double x, double y);
    void handle_keyboard_event(int key, int scancode, int action, int modifiers);
    void handle_gamepad_event(GLFWgamepadstate& gamepad);
    void handle_rumple(unsigned short low_freq_motor, unsigned short high_freq_motor);
    void stop_rumple();
    
    bool gamepad_combo_is_enabled(GamepadCombo combo);
    
    void send_to_stream() {
        send_mouse_to_stream();
        send_keyboard_to_stream();
        send_gamepad_to_stream();
    }
    
private:
    InputController();
    
    void send_mouse_to_stream();
    void send_keyboard_to_stream();
    void send_gamepad_to_stream();
    
    bool gamepad_button_is_enabled(int button);
    bool gamepad_trigger_is_enabled(int trigger);
};
