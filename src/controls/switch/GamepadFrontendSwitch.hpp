#include "GamepadFrontend.hpp"
#include <switch.h>
#pragma once

class GamepadFrontendSwitch: public GamepadFrontend {
public:
    GamepadFrontendSwitch();
    
    void handle_gamepad();
    
    void handle_rumple(unsigned short controller, unsigned short low_freq_motor, unsigned short high_freq_motor);
    void stop_rumple();
    
    GamepadState gamepad_state() const {
        return m_gamepad_state;
    }
    
private:
    PadState m_pad_state = {0};
    GamepadState m_gamepad_state = {0};
    
    HidVibrationDeviceHandle m_vibration_device_handles[2][2];
    HidVibrationValue m_vibration_values[2];
    
    bool set_new_pad_state(PadState& state);
};
