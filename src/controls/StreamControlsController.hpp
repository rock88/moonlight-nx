#include "Singleton.hpp"
#include "MouseController.hpp"
#include "KeyboardController.hpp"
#include "GamepadController.hpp"
#pragma once

class StreamControlsController: public Singleton<StreamControlsController> {
public:
    void handle_rumple(unsigned short controller, unsigned short low_freq_motor, unsigned short high_freq_motor);
    void stop_rumple();
    
    void send_to_stream(int width, int height);
    
    bool should_exit() const;
    bool should_exit_and_close() const;
    bool should_show_stats() const;
    
private:
    MouseState m_mouse_state = {0};
    KeyboardState m_keyboard_state;
    GamepadState m_gamepad_state = {0};
    
    bool m_mouse_is_pressed = false, m_mouse_is_released = false;
    
    bool m_press_escape = false;
    bool m_press_alt_enter = false;
    bool m_press_alt_tab = false;
    bool m_press_shift_tab = false;
    bool m_press_win_key = false;
};
