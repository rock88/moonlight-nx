#include "Singleton.hpp"
#include <switch.h>
#pragma once

// Moonlight ready gamepad
struct GamepadState {
    short buttonFlags;
    unsigned char leftTrigger;
    unsigned char rightTrigger;
    short leftStickX;
    short leftStickY;
    short rightStickX;
    short rightStickY;
    
    bool is_equal(GamepadState other) {
        return buttonFlags == other.buttonFlags &&
        leftTrigger == other.leftTrigger &&
        rightTrigger == other.rightTrigger &&
        leftStickX == other.leftStickX &&
        leftStickY == other.leftStickY &&
        rightStickX == other.rightStickX &&
        rightStickY == other.rightStickY;
    }
};

class GamepadController: public Singleton<GamepadController> {
public:
    void init();
    
    void handle_gamepad();
    
    void handle_rumple(unsigned short controller, unsigned short low_freq_motor, unsigned short high_freq_motor);
    void stop_rumple();
    
    const GamepadState& gamepad_state() {
        return m_gamepad_state;
    }
    
private:
    PadState m_pad_state;
    GamepadState m_gamepad_state = {0};
    
    bool set_new_pad_state(PadState& state);
    
    HidVibrationDeviceHandle m_vibration_device_handles[2][2];
    HidVibrationValue m_vibration_values[2];
};
