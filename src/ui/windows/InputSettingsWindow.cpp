#include "InputSettingsWindow.hpp"
#include <nanogui/opengl.h>
#include <functional>

using namespace nanogui;

class GamepadInputOverlay: Widget {
public:
    GamepadInputOverlay(Widget* parent, std::string caption, int button_count = 1): Widget(parent) {
        m_button_count = button_count;
        m_caption = caption;
        set_fixed_size(parent->screen()->size());
        screen()->perform_layout();
    }
    
    bool gamepad_button_event(int jid, int button, int action) override {
        if (action) {
            GamepadButtons gamepad_button = GamepadMapper::mapper()->convert_nanogui_gamepad_button(button);
            handle_button_input(button, gamepad_button);
        }
        return false;
    }
    
    bool gamepad_analog_event(int jid, int axis, float value) override {
        if (value > 0) {
            GamepadButtons gamepad_button = GamepadMapper::mapper()->convert_nanogui_analog_axis(axis);
            handle_button_input(-1, gamepad_button);
        }
        return false;
    }
    
    bool is_done() const {
        bool result = false;
        
        for (int i = 0; i < m_button_count; i++) {
            result = m_buttons[i] != GamepadButtonUnknown;
        }
        return result;
    }
    
    void handle_button_input(int button, GamepadButtons gamepad_button) {
        if (gamepad_button == GamepadButtonUnknown) {
            return;
        }
        
        if (is_done()) {
            if (button == NANOGUI_GAMEPAD_BUTTON_A) {
                if (m_completion) {
                    inc_ref();
                    m_completion(m_buttons);
                    dec_ref();
                }
            } else if (button == NANOGUI_GAMEPAD_BUTTON_B) {
                m_buttons[0] = GamepadButtonUnknown;
                m_buttons[1] = GamepadButtonUnknown;
                m_buttons[2] = GamepadButtonUnknown;
            }
        } else {
            if (m_buttons[0] == GamepadButtonUnknown) {
                m_buttons[0] = gamepad_button;
            } else if (m_buttons[1] == GamepadButtonUnknown && m_buttons[0] != gamepad_button) {
                m_buttons[1] = gamepad_button;
            } else if (m_buttons[2] == GamepadButtonUnknown && m_buttons[0] != gamepad_button && m_buttons[1] != gamepad_button) {
                m_buttons[2] = gamepad_button;
            }
        }
    }
    
    void draw(NVGcontext *ctx) override {
        nvgSave(ctx);
        
        // Draw bg
        nvgFillColor(ctx, Color(0, 0, 0, 220));
        nvgBeginPath(ctx);
        nvgRect(ctx, 0, 0, width(), height());
        nvgFill(ctx);
        
        // Draw text
        nvgFillColor(ctx, Color(255, 255, 255, 200));
        nvgFontSize(ctx, 30);
        nvgFontFace(ctx, "sans");
        nvgTextAlign(ctx, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
        nvgText(ctx, width() / 2, height() / 2 - 80, m_caption.c_str(), NULL);
        
        if (m_buttons[0] != GamepadButtonUnknown) {
            std::string string = "\"" + GamepadMapper::mapper()->button_label(m_buttons[0], true) + "\"";
            
            if (m_buttons[1] != GamepadButtonUnknown) {
                string += " + \"" + GamepadMapper::mapper()->button_label(m_buttons[1], true) + "\"";
            }
            
            if (m_buttons[2] != GamepadButtonUnknown) {
                string += " + \"" + GamepadMapper::mapper()->button_label(m_buttons[2], true) + "\"";
            }
            
            nvgText(ctx, width() / 2, height() / 2, string.c_str(), NULL);
            
            if (is_done()) {
                string = "Press \"" +
                    GamepadMapper::mapper()->button_label(GamepadButtonA, true) +
                    "\" for Accept, \"" +
                    GamepadMapper::mapper()->button_label(GamepadButtonB, true) +
                    "\" for Cancel.";
                nvgText(ctx, width() / 2, height() / 2 + 80, string.c_str(), NULL);
            }
        }
        
        nvgRestore(ctx);
    }
    
    void set_completion(std::function<void(std::array<GamepadButtons, 3>)> completion) {
        m_completion = completion;
    }
    
    void dispose() {
        screen()->remove_child(this);
    }
    
private:
    int m_button_count;
    std::string m_caption;
    std::function<void(std::array<GamepadButtons, 3>)> m_completion;
    std::array<GamepadButtons, 3> m_buttons = {GamepadButtonUnknown, GamepadButtonUnknown, GamepadButtonUnknown};
};

static GamepadInputOverlay* overlay = nullptr;

InputSettingsWindow::InputSettingsWindow(Widget* parent, int app_id, std::string app_name): ContentWindow(parent, "Input Settings (" + app_name + ")") {
    m_app_id = app_id;
    GamepadMapper::mapper()->load_gamepad_map(m_app_id);
    
    set_left_pop_button();
    set_box_layout(Orientation::Vertical, Alignment::Minimum);
    reload_gamepad_input_settings();
}

Widget* InputSettingsWindow::new_container(Widget* parent, int width) {
    auto container = parent->add<Widget>();
    container->set_layout(new GroupLayout(30, 10, 30, 10));
    container->set_fixed_width(width);
    return container;
}

void InputSettingsWindow::reload_gamepad_input_settings() {
    clean_container();
    clean_right_title_buttons();
    
    set_right_title_button(FA_UNDO, [this] {
        reset();
        reload_gamepad_input_settings();
    });
    set_right_title_button(FA_KEYBOARD, [this] {
        reload_combo_settings();
    });
    
    container()->add<Label>("Gamepad mapping (Switch -> X360)");
    
    auto content = container()->add<Widget>();
    content->set_layout(new BoxLayout(Orientation::Horizontal, Alignment::Minimum, 30, 10));
    auto button_container = new_container(content);
    
    for (int i = 0; i < GamepadMapper::mapper()->button_count(); i++) {
        if (i == GamepadButtonUp) {
            button_container = new_container(content);
        }
        
        if (i == GamepadButtonPlus) {
            button_container = new_container(content);
        }
        
        if (i == GamepadButtonL) {
            button_container = new_container(content);
        }
        
        button_container->add<Label>(GamepadMapper::mapper()->button_label((GamepadButtons)i, false));
        auto button = button_container->add<Button>(GamepadMapper::mapper()->button_label(GamepadMapper::mapper()->mapped_button((GamepadButtons)i), true));
        button->set_callback([this, i] {
            assign_button((GamepadButtons)i);
        });
    }
    perform_layout();
}

void InputSettingsWindow::reload_combo_settings() {
    clean_container();
    clean_right_title_buttons();
    
    set_right_title_button(FA_UNDO, [this] {
        reset();
        reload_combo_settings();
    });
    set_right_title_button(FA_GAMEPAD, [this] {
        reload_gamepad_input_settings();
    });
    
    container()->add<Label>("Keyboard Combo mapping");
    
    auto content = container()->add<Widget>();
    content->set_layout(new BoxLayout(Orientation::Horizontal, Alignment::Minimum, 30, 10));
    auto button_container = new_container(content, 360);
    
    for (int i = 0; i < GamepadMapper::mapper()->combo_count(); i++) {
        if (i == GamepadComboEscape) {
            button_container = new_container(content, 360);
        }
        
        auto buttons = GamepadMapper::mapper()->combo_buttons((GamepadCombo)i);
        
        std::string combo_text = "";
        
        for (int i = 0; i < 3; i++) {
            if (buttons[i] != GamepadButtonUnknown) {
                combo_text += GamepadMapper::mapper()->button_label(GamepadMapper::mapper()->mapped_button(buttons[i]), true);
                
                if (i < 2 && buttons[i + 1] != GamepadButtonUnknown) {
                    combo_text += " + ";
                }
            }
        }
        
        button_container->add<Label>(GamepadMapper::mapper()->combo_label((GamepadCombo)i));
        auto button = button_container->add<Button>(combo_text);
        button->set_callback([this, i] {
            assign_combo((GamepadCombo)i, i == GamepadComboGuide ? 2 : 3);
        });
    }
    
    perform_layout();
}

void InputSettingsWindow::reset() {
    if (m_app_id == 0) {
        GamepadMapper::mapper()->load_defaults_gamepad_map();
    } else {
        GamepadMapper::mapper()->load_gamepad_map(0);
    }
    
    GamepadMapper::mapper()->save_gamepad_map(m_app_id);
}

bool InputSettingsWindow::mouse_button_event(const Vector2i &p, int button, bool down, int modifiers) {
    if (overlay) {
        return false;
    }
    return ContentWindow::mouse_button_event(p, button, down, modifiers);
}

bool InputSettingsWindow::gamepad_button_event(int jid, int button, int action) {
    if (overlay) {
        return overlay->gamepad_button_event(jid, button, action);
    }
    return ContentWindow::gamepad_button_event(jid, button, action);
}

bool InputSettingsWindow::gamepad_analog_event(int jid, int axis, float value) {
    if (overlay) {
        return overlay->gamepad_analog_event(jid, axis, value);
    }
    return ContentWindow::gamepad_button_event(jid, axis, value);
}

void InputSettingsWindow::window_disappear() {
    if (m_has_changes) {
        GamepadMapper::mapper()->save_gamepad_map(m_app_id);
    }
}

void InputSettingsWindow::assign_button(GamepadButtons button) {
    GamepadButtons current = GamepadMapper::mapper()->mapped_button(button);
    
    overlay = screen()->add<GamepadInputOverlay>("Press button for reassign \"" + GamepadMapper::mapper()->button_label(button, false) + "\"");
    overlay->set_completion([this, button, current](auto result) {
        overlay->dispose();
        overlay = NULL;
        
        if (current != result[0]) {
            m_has_changes = true;
            GamepadMapper::mapper()->set_mapped_button(button, result[0]);
            reload_gamepad_input_settings();
        }
    });
}

void InputSettingsWindow::assign_combo(GamepadCombo combo, int buttons_count) {
    auto current = GamepadMapper::mapper()->combo_buttons(combo);
    
    overlay = screen()->add<GamepadInputOverlay>("Enter combo for reassign \"" + GamepadMapper::mapper()->combo_label(combo) + "\"", buttons_count);
    overlay->set_completion([this, combo, current](auto result) {
        overlay->dispose();
        overlay = NULL;
        
        if (current != result) {
            m_has_changes = true;
            GamepadMapper::mapper()->set_combo_buttons(result, combo);
            reload_combo_settings();
        }
    });
}
