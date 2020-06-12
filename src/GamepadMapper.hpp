#include <stdio.h>
#include <string>
#include <map>
#include <array>
#pragma once

struct GLFWgamepadstate;

enum GamepadButtons: int {
    GamepadButtonUnknown = -1,
    GamepadButtonA,
    GamepadButtonB,
    GamepadButtonX,
    GamepadButtonY,
    GamepadButtonUp,
    GamepadButtonDown,
    GamepadButtonLeft,
    GamepadButtonRight,
    GamepadButtonPlus,
    GamepadButtonMinus,
    GamepadButtonLeftThumb,
    GamepadButtonRightThumb,
    GamepadButtonL,
    GamepadButtonR,
    GamepadButtonZL,
    GamepadButtonZR,
    GamepadButtonLast
};

enum GamepadCombo: int {
    GamepadComboGuide,
    GamepadComboExit,
    GamepadComboExitAndClose,
    GamepadComboAltEnter,
    GamepadComboEscape,
    GamepadComboShowStats,
    GamepadComboHideStats,
    GamepadComboLast
};

class GamepadMapper {
public:
    static GamepadMapper* mapper() {
        static GamepadMapper mapper;
        return &mapper;
    }
    
    void load_gamepad_map(int app_id);
    void save_gamepad_map(int app_id);
    
    bool gamepad_combo_is_enabled(GLFWgamepadstate& gamepad, GamepadCombo combo);
    
    GLFWgamepadstate map(GLFWgamepadstate& gamepad);
    
    GamepadButtons convert_nanogui_gamepad_button(int button);
    GamepadButtons convert_nanogui_analog_axis(int axis);
    
    void map_button(GamepadButtons origin_button, GamepadButtons new_button);
    GamepadButtons mapped_button(int origin_button);
    
    void set_combo_buttons(std::array<GamepadButtons, 3> buttons, GamepadCombo combo);
    std::array<GamepadButtons, 3> combo_buttons(GamepadCombo combo);
    
    int button_count() const {
        return m_button_count;
    }
    
    int combo_count() const {
        return m_combo_count;
    }
    
    std::string button_label(GamepadButtons button, bool use_switch_label);
    std::string combo_label(GamepadCombo combo);
    
private:
    GamepadMapper();
    
    void load_defaults();
    
    const int m_button_count = GamepadButtonLast;
    const int m_combo_count = GamepadComboLast;
    
    GamepadButtons m_gamepad_map[GamepadButtonLast];
    std::map<GamepadCombo, std::array<GamepadButtons, 3>> m_combo;
};
