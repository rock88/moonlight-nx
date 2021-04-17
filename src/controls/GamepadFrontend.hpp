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

class GamepadFrontend {
public:
    virtual void handle_gamepad() = 0;
    virtual void handle_rumple(unsigned short controller, unsigned short low_freq_motor, unsigned short high_freq_motor) = 0;
    virtual void stop_rumple() = 0;
    virtual GamepadState gamepad_state() const = 0;
};
