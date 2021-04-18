#include "GamepadFrontend.hpp"
#pragma once

class DummyGamepadFrontend: public GamepadFrontend {
public:
    GamepadFrontendDummy() {}
    
    void handle_gamepad() {}
    
    void handle_rumple(unsigned short controller, unsigned short low_freq_motor, unsigned short high_freq_motor) {}
    void stop_rumple() {}
    
    GamepadState gamepad_state() const {
        return m_gamepad_state;
    }
    
private:
    GamepadState m_gamepad_state = {0};
};
