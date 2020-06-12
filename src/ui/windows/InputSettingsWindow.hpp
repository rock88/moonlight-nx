#include "ContentWindow.hpp"
#include "GamepadMapper.hpp"
#pragma once

class InputSettingsWindow: public ContentWindow {
public:
    InputSettingsWindow(nanogui::Widget* parent);
    
    bool mouse_button_event(const nanogui::Vector2i &p, int button, bool down, int modifiers) override;
    bool gamepad_button_event(int jid, int button, int action) override;
    bool gamepad_analog_event(int jid, int axis, float value) override;
    
private:
    nanogui::Widget* new_container(nanogui::Widget* parent, int width = 240);
    
    void reload_gamepad_input_settings();
    void reload_combo_settings();
    
    void assign_button(GamepadButtons button);
    void assign_combo(GamepadCombo combo, int buttons_count);
};
