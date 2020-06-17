#include "GamepadMapper.hpp"
#include "Settings.hpp"
#include <jansson.h>
#include <nanogui/opengl.h>
#include <GLFW/glfw3.h>
#include <vector>

static int gamepad_button_to_nanogui_button(GamepadButtons button) {
    switch (button) {
        case GamepadButtonA: return NANOGUI_GAMEPAD_BUTTON_A;
        case GamepadButtonB: return NANOGUI_GAMEPAD_BUTTON_B;
        case GamepadButtonX: return NANOGUI_GAMEPAD_BUTTON_X;
        case GamepadButtonY: return NANOGUI_GAMEPAD_BUTTON_Y;
        case GamepadButtonUp: return NANOGUI_GAMEPAD_BUTTON_DPAD_UP;
        case GamepadButtonDown: return NANOGUI_GAMEPAD_BUTTON_DPAD_DOWN;
        case GamepadButtonLeft: return NANOGUI_GAMEPAD_BUTTON_DPAD_LEFT;
        case GamepadButtonRight: return NANOGUI_GAMEPAD_BUTTON_DPAD_RIGHT;
        case GamepadButtonPlus: return NANOGUI_GAMEPAD_BUTTON_START;
        case GamepadButtonMinus: return NANOGUI_GAMEPAD_BUTTON_BACK;
        case GamepadButtonLeftThumb: return NANOGUI_GAMEPAD_BUTTON_LEFT_THUMB;
        case GamepadButtonRightThumb: return NANOGUI_GAMEPAD_BUTTON_RIGHT_THUMB;
        case GamepadButtonL: return NANOGUI_GAMEPAD_BUTTON_LEFT_BUMPER;
        case GamepadButtonR: return NANOGUI_GAMEPAD_BUTTON_RIGHT_BUMPER;
        default: return 0;
    }
}

GamepadMapper::GamepadMapper() {
    load_defaults_gamepad_map();
}

GLFWgamepadstate GamepadMapper::map(GLFWgamepadstate &gamepad) {
    GLFWgamepadstate result = gamepad;
    
    for (int i = 0; i < NANOGUI_GAMEPAD_BUTTON_DPAD_LEFT + 1; i++) {
        if (m_gamepad_map[i] < GamepadButtonZL) {
            result.buttons[gamepad_button_to_nanogui_button(m_gamepad_map[i])] = gamepad.buttons[gamepad_button_to_nanogui_button((GamepadButtons)i)];
        } else if (i < GamepadButtonZL && m_gamepad_map[i] == GamepadButtonZL) {
            result.axes[NANOGUI_GAMEPAD_AXIS_LEFT_TRIGGER] = gamepad.buttons[gamepad_button_to_nanogui_button((GamepadButtons)i)] ? 1 : -1;
        } else if (i < GamepadButtonZL && m_gamepad_map[i] == GamepadButtonZR) {
            result.axes[NANOGUI_GAMEPAD_AXIS_RIGHT_TRIGGER] = gamepad.buttons[gamepad_button_to_nanogui_button((GamepadButtons)i)] ? 1 : -1;
        }
    }
    
    if (m_gamepad_map[GamepadButtonZL] < GamepadButtonZL) {
        result.buttons[gamepad_button_to_nanogui_button(m_gamepad_map[GamepadButtonZL])] = gamepad.axes[NANOGUI_GAMEPAD_AXIS_LEFT_TRIGGER] > 0;
    } else if (m_gamepad_map[GamepadButtonZL] == GamepadButtonZR) {
        result.axes[NANOGUI_GAMEPAD_AXIS_RIGHT_TRIGGER] = gamepad.axes[NANOGUI_GAMEPAD_AXIS_LEFT_TRIGGER];
    }
    
    if (m_gamepad_map[GamepadButtonZR] < GamepadButtonZL) {
        result.buttons[gamepad_button_to_nanogui_button(m_gamepad_map[GamepadButtonZR])] = gamepad.axes[NANOGUI_GAMEPAD_AXIS_RIGHT_TRIGGER] > 0;
    } else if (m_gamepad_map[GamepadButtonZR] == GamepadButtonZL) {
        result.axes[NANOGUI_GAMEPAD_AXIS_LEFT_TRIGGER] = gamepad.axes[NANOGUI_GAMEPAD_AXIS_RIGHT_TRIGGER];
    }
    
    return result;
}

GamepadButtons GamepadMapper::convert_nanogui_gamepad_button(int button) {
    switch (button) {
        case NANOGUI_GAMEPAD_BUTTON_A: return GamepadButtonA;
        case NANOGUI_GAMEPAD_BUTTON_B: return GamepadButtonB;
        case NANOGUI_GAMEPAD_BUTTON_X: return GamepadButtonX;
        case NANOGUI_GAMEPAD_BUTTON_Y: return GamepadButtonY;
        case NANOGUI_GAMEPAD_BUTTON_DPAD_UP: return GamepadButtonUp;
        case NANOGUI_GAMEPAD_BUTTON_DPAD_DOWN: return GamepadButtonDown;
        case NANOGUI_GAMEPAD_BUTTON_DPAD_LEFT: return GamepadButtonLeft;
        case NANOGUI_GAMEPAD_BUTTON_DPAD_RIGHT: return GamepadButtonRight;
        case NANOGUI_GAMEPAD_BUTTON_START: return GamepadButtonPlus;
        case NANOGUI_GAMEPAD_BUTTON_BACK: return GamepadButtonMinus;
        case NANOGUI_GAMEPAD_BUTTON_LEFT_THUMB: return GamepadButtonLeftThumb;
        case NANOGUI_GAMEPAD_BUTTON_RIGHT_THUMB: return GamepadButtonRightThumb;
        case NANOGUI_GAMEPAD_BUTTON_LEFT_BUMPER: return GamepadButtonL;
        case NANOGUI_GAMEPAD_BUTTON_RIGHT_BUMPER: return GamepadButtonR;
        default: return GamepadButtonUnknown;
    }
}

GamepadButtons GamepadMapper::convert_nanogui_analog_axis(int axis) {
    switch (axis) {
        case NANOGUI_GAMEPAD_AXIS_LEFT_TRIGGER: return GamepadButtonZL;
        case NANOGUI_GAMEPAD_AXIS_RIGHT_TRIGGER: return GamepadButtonZR;
        default: return GamepadButtonUnknown;
    }
}

void GamepadMapper::set_mapped_button(GamepadButtons origin_button, GamepadButtons new_button) {
    m_gamepad_map[origin_button] = new_button;
}

GamepadButtons GamepadMapper::mapped_button(GamepadButtons button) {
    return m_gamepad_map[button];
}

void GamepadMapper::set_combo_buttons(std::array<GamepadButtons, 3> buttons, GamepadCombo combo) {
    m_combo[combo] = buttons;
}

std::array<GamepadButtons, 3> GamepadMapper::combo_buttons(GamepadCombo combo) {
    return m_combo[combo];
}

bool GamepadMapper::gamepad_combo_is_enabled(GLFWgamepadstate& gamepad, GamepadCombo combo) {
    if (m_combo.find(combo) != m_combo.end()) {
        bool result = true;
        
        for (int i = 0; i < 3; i++) {
            GamepadButtons button = m_combo[combo][i];
            
            if (button == GamepadButtonUnknown) {
                continue;
            }
            
            if (button < GamepadButtonZL) {
                result = result && gamepad.buttons[gamepad_button_to_nanogui_button(button)];
            } else if (button == GamepadButtonZL) {
                result = result && (gamepad.axes[NANOGUI_GAMEPAD_AXIS_LEFT_TRIGGER] == 1);
            } else if (button == GamepadButtonZR) {
                result = result && (gamepad.axes[NANOGUI_GAMEPAD_AXIS_RIGHT_TRIGGER] == 1);
            }
        }
        return result;
    }
    return false;
}

std::string GamepadMapper::button_label(GamepadButtons button, bool use_switch_label) {
    static std::map<GamepadButtons, std::vector<std::string>> map = {
        {GamepadButtonA, {"A"}},
        {GamepadButtonB, {"B"}},
        {GamepadButtonX, {"X"}},
        {GamepadButtonY, {"Y"}},
        {GamepadButtonUp, {"Up"}},
        {GamepadButtonDown, {"Down"}},
        {GamepadButtonLeft, {"Left"}},
        {GamepadButtonRight, {"Right"}},
        {GamepadButtonPlus, {"Start", "Plus"}},
        {GamepadButtonMinus, {"Back", "Minus"}},
        {GamepadButtonLeftThumb, {"Left Thumb"}},
        {GamepadButtonRightThumb, {"Right Thumb"}},
        {GamepadButtonL, {"LB", "L"}},
        {GamepadButtonR, {"RB", "R"}},
        {GamepadButtonZL, {"LT", "ZL"}},
        {GamepadButtonZR, {"RT", "ZR"}}
    };
    
    if (map.find(button) != map.end()) {
        return use_switch_label ? map[button].back() : map[button].front();
    }
    return "N/A";
}

std::string GamepadMapper::combo_label(GamepadCombo combo) {
    static std::map<GamepadCombo, std::string> map = {
        {GamepadComboGuide, "Guide"},
        {GamepadComboExit, "Exit"},
        {GamepadComboExitAndClose, "Exit And Close"},
        {GamepadComboAltEnter, "Alt + Enter"},
        {GamepadComboEscape, "Escape"},
        {GamepadComboShowStats, "Show Stats"},
        {GamepadComboHideStats, "Hide Stats"}
    };
    
    if (map.find(combo) != map.end()) {
        return map[combo];
    }
    return "N/A";
}

void GamepadMapper::load_defaults_gamepad_map() {
    for (int i = 0; i < GamepadButtonLast; i++) {
        m_gamepad_map[i] = (GamepadButtons)i;
    }
    
    // Swap A/B and X/Y
    m_gamepad_map[GamepadButtonA] = GamepadButtonB;
    m_gamepad_map[GamepadButtonB] = GamepadButtonA;
    m_gamepad_map[GamepadButtonX] = GamepadButtonY;
    m_gamepad_map[GamepadButtonY] = GamepadButtonX;
    
    m_combo.clear();
    m_combo[GamepadComboGuide] = {GamepadButtonMinus, GamepadButtonPlus, GamepadButtonUnknown};
    m_combo[GamepadComboExit] = {GamepadButtonL, GamepadButtonR, GamepadButtonUp};
    m_combo[GamepadComboExitAndClose] = {GamepadButtonL, GamepadButtonR, GamepadButtonDown};
    m_combo[GamepadComboAltEnter] = {GamepadButtonL, GamepadButtonR, GamepadButtonLeft};
    m_combo[GamepadComboEscape] = {GamepadButtonL, GamepadButtonR, GamepadButtonRight};
    m_combo[GamepadComboShowStats] = {GamepadButtonZL, GamepadButtonZR, GamepadButtonLeft};
    m_combo[GamepadComboHideStats] = {GamepadButtonZL, GamepadButtonZR, GamepadButtonRight};
}

void GamepadMapper::load_gamepad_map(int app_id) {
    load_defaults_gamepad_map();
    
    json_t* root = json_load_file(Settings::settings()->gamepad_mapping_path().c_str(), 0, NULL);
    
    if (root != NULL) {
        if (json_t* mapping = json_object_get(root, std::to_string(app_id).c_str())) {
            if (json_t* buttons = json_object_get(mapping, "buttons")) {
                size_t size = std::min(json_array_size(buttons), (size_t)GamepadButtonLast);
                
                for (size_t i = 0; i < size; i++) {
                    if (json_t* button = json_array_get(buttons, i)) {
                        if (json_typeof(button) == JSON_INTEGER) {
                            m_gamepad_map[i] = (GamepadButtons)json_integer_value(button);
                        }
                    }
                }
            }
            
            if (json_t* combo = json_object_get(mapping, "combo")) {
                for (int i = 0; i < combo_count(); i++) {
                    if (json_t* buttons = json_object_get(combo, combo_label((GamepadCombo)i).c_str())) {
                        size_t size = std::min(json_array_size(buttons), (size_t)3);
                        
                        for (int j = 0; j < size; j++) {
                            if (json_t* button = json_array_get(buttons, j)) {
                                if (json_typeof(button) == JSON_INTEGER) {
                                    m_combo[(GamepadCombo)i][(GamepadButtons)j] = (GamepadButtons)json_integer_value(button);
                                }
                            }
                        }
                    }
                }
            }
        }
        
        json_decref(root);
    }
}

void GamepadMapper::save_gamepad_map(int app_id) {
    json_t* root = json_load_file(Settings::settings()->gamepad_mapping_path().c_str(), 0, NULL);
    
    if (root == NULL) {
        root = json_object();
    }
    
    if (root) {
        if (json_t* mapping = json_object()) {
            if (json_t* buttons = json_array()) {
                for (int i = 0; i < GamepadButtonLast; i++) {
                    json_array_append(buttons, json_integer(m_gamepad_map[i]));
                }
                json_object_set(mapping, "buttons", buttons);
            }
            json_object_set(root, std::to_string(app_id).c_str(), mapping);
            
            if (json_t* combo = json_object()) {
                for (int i = 0; i < combo_count(); i++) {
                    if (json_t* buttons = json_array()) {
                        for (auto button: combo_buttons((GamepadCombo)i)) {
                            json_array_append(buttons, json_integer(button));
                        }
                        json_object_set(combo, combo_label((GamepadCombo)i).c_str(), buttons);
                    }
                }
                json_object_set(mapping, "combo", combo);
            }
        }
        
        json_dump_file(root, Settings::settings()->gamepad_mapping_path().c_str(), JSON_INDENT(4));
        json_decref(root);
    }
}
