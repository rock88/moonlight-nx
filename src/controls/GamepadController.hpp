#include "Singleton.hpp"
#include "GamepadFrontend.hpp"
#pragma once

class GamepadController: public Singleton<GamepadController> {
public:
    void init(GamepadFrontend* frontend);
    
    void handle_gamepad();
    
    void handle_rumple(unsigned short controller, unsigned short low_freq_motor, unsigned short high_freq_motor);
    void stop_rumple();
    
    const GamepadState& gamepad_state() {
        return m_gamepad_state;
    }
    
private:
    GamepadState m_gamepad_state = {0};
    int m_nanogui_gamepad_state[14] = {0};
    GamepadFrontend* m_frontend;
};
