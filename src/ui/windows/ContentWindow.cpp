#include "ContentWindow.hpp"
#include "Application.hpp"
#include "LoadingOverlay.hpp"
#include "nanovg.h"
#include <nanogui/opengl.h>

using namespace nanogui;

ContentWindow::ContentWindow(Widget *parent, const std::string& title): Widget(parent) {
    set_layout(new BoxLayout(Orientation::Vertical, Alignment::Middle, 0, 0));
    set_size(parent->size());
    set_fixed_size(parent->size());
    
    auto title_container = add<Widget>();
    title_container->set_fixed_size(Size(parent->width() - 40, 80));
    title_container->set_layout(new BoxLayout(Orientation::Horizontal, Alignment::Middle, 0, 10));
    
    m_left_title_button_container = title_container->add<Widget>();
    m_left_title_button_container->set_layout(new BoxLayout(Orientation::Horizontal));
    
    m_title_label = title_container->add<Label>(title);
    m_title_label->set_fixed_width(parent->width() - 140);
    m_title_label->set_font_size(40);
    
    m_right_title_button_container = title_container->add<Widget>();
    m_right_title_button_container->set_layout(new BoxLayout(Orientation::Horizontal));
    
    m_scroll = add<VScrollPanel>();
    m_scroll->set_fixed_size(Size(parent->width() - 60, parent->height() - 80));
    m_container = m_scroll->add<Widget>();
}

void ContentWindow::draw(NVGcontext *ctx) {
    nvgSave(ctx);
    
    // Draw bg
    nvgFillColor(ctx, Color(48, 48, 48, 255));
    nvgBeginPath(ctx);
    nvgRect(ctx, 0, 0, width(), height());
    nvgFill(ctx);
    
    // Draw header
    nvgFillColor(ctx, Color(62, 78, 184, 255));
    nvgBeginPath(ctx);
    nvgRect(ctx, 0, 0, width(), 80);
    nvgFill(ctx);
    
    // Draw separator
    nvgBeginPath(ctx);
    NVGpaint gradient = nvgLinearGradient(ctx, 0, 80, 0, 84, Color(0, 0, 0, 100), Color(0, 0, 0, 0));
    nvgFillPaint(ctx, gradient);
    nvgRect(ctx, 0, 80, width(), 4);
    nvgFill(ctx);
    
    nvgRestore(ctx);
    
    Widget::draw(ctx);
}

void ContentWindow::set_left_title_button(int icon, const std::function<void()> &callback) {
    auto button = m_left_title_button_container->add<Button>("", icon);
    button->set_fixed_size(Size(60, 60));
    button->set_icon_extra_scale(3);
    button->set_callback([callback] {
        callback();
    });
    perform_layout();
}

void ContentWindow::set_right_title_button(int icon, const std::function<void()> &callback) {
    auto button = m_right_title_button_container->add<Button>("", icon);
    button->set_fixed_size(Size(60, 60));
    button->set_icon_extra_scale(3);
    button->set_callback([callback] {
        callback();
    });
    perform_layout();
}

static inline std::vector<Widget *> selectables_child_recursive(Widget *widget) {
    std::vector<Widget *> selectables;
    
    for (auto child: widget->children()) {
        if (child->selectable()) {
            selectables.push_back(child);
        }
        auto child_selectables = selectables_child_recursive(child);
        selectables.insert(selectables.end(), child_selectables.begin(), child_selectables.end());
    }
    return selectables;
}

static inline Widget *most_closed_widget(Widget *target, std::vector<Widget *> widgets, bool left, bool right, bool up, bool down) {
    if (widgets.size() < 2) {
        return nullptr;
    }
    
    int index = -1;
    int distanse = std::numeric_limits<int>::max();
    
    for (int i = 0; i < widgets.size(); i++) {
        auto widget = widgets[i];
        
        if (widget == target) {
            continue;
        }
        
        if (left && widget->center().y() == target->center().y() && widget->center().x() < target->center().x()) {
            int new_distanse = target->center().x() - widget->center().x();
            if (new_distanse < distanse) {
                distanse = new_distanse;
                index = i;
            }
        } else if (right && widget->center().y() == target->center().y() && widget->center().x() > target->center().x()) {
            int new_distanse = widget->center().x() - target->center().x();
            if (new_distanse < distanse) {
                distanse = new_distanse;
                index = i;
            }
        } else if (up && widget->center().x() == target->center().x() && widget->center().y() < target->center().y()) {
            int new_distanse = target->center().y() - widget->center().y();
            if (new_distanse < distanse) {
                distanse = new_distanse;
                index = i;
            }
        } else if (down && widget->center().x() == target->center().x() && widget->center().y() > target->center().y()) {
            int new_distanse = widget->center().y() - target->center().y();
            if (new_distanse < distanse) {
                distanse = new_distanse;
                index = i;
            }
        }
    }
    
    if (index != -1) {
        return widgets[index];
    }
    return nullptr;
}

bool ContentWindow::gamepad_button_event(int jid, int button, int action) {
    if (!action) {
        return false;
    }
    
    std::vector<MessageDialog *> messages;
    std::vector<LoadingOverlay *> loaders;
    
    for (auto child: screen()->children()) {
        if (auto message = dynamic_cast<MessageDialog *>(child)) {
            messages.push_back(message);
        }
        if (auto loader = dynamic_cast<LoadingOverlay *>(child)) {
            loaders.push_back(loader);
        }
    }
    
    bool handle_button = messages.empty() && loaders.empty();
    
    if (button == NANOGUI_GAMEPAD_BUTTON_B) {
        if (!messages.empty()) {
            for (auto message: messages) {
                message->dispose();
            }
            return false;
        }
        
        if (!loaders.empty()) {
            return false;
        }
        
        pop();
        return false;
    } else if (handle_button && button == NANOGUI_GAMEPAD_BUTTON_A) {
        auto selectables = selectables_child_recursive(m_container);
        auto current = std::find_if(selectables.begin(), selectables.end(), [](auto c) { return c->selected(); });
        
        if (current != selectables.end()) {
            auto widget = *current;
            widget->mouse_button_event(widget->position() + widget->size() / 2, NANOGUI_MOUSE_BUTTON_1, 1, 0);
            widget->mouse_button_event(widget->position() + widget->size() / 2, NANOGUI_MOUSE_BUTTON_1, 0, 0);
        }
        return false;
    }
    
    auto selectables = selectables_child_recursive(m_container);
    auto current = std::find_if(selectables.begin(), selectables.end(), [](auto c) { return c->selected(); });
    
    if (handle_button && button >= NANOGUI_GAMEPAD_BUTTON_DPAD_UP && button <= NANOGUI_GAMEPAD_BUTTON_DPAD_LEFT) {
        if (current == selectables.end()) {
            selectables.front()->set_selected(true);
            return false;
        }
        
        auto current_selectable = *current;
        auto new_selectable = most_closed_widget(
            current_selectable,
            selectables,
            button == NANOGUI_GAMEPAD_BUTTON_DPAD_LEFT,
            button == NANOGUI_GAMEPAD_BUTTON_DPAD_RIGHT,
            button == NANOGUI_GAMEPAD_BUTTON_DPAD_UP,
            button == NANOGUI_GAMEPAD_BUTTON_DPAD_DOWN
            );
        
        if (new_selectable) {
            current_selectable->set_selected(false);
            new_selectable->set_selected(true);
            
            m_scroll->set_scroll((new_selectable->position().y() + new_selectable->height()) / m_scroll->height());
        }
    }
    return false;
}
