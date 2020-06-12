#include "ContentWindow.hpp"
#include "Application.hpp"
#include "LoadingOverlay.hpp"
#include "nanovg.h"
#include <nanogui/opengl.h>
#include <math.h>

using namespace nanogui;

ContentWindow::ContentWindow(Widget *parent, const std::string& title): Widget(parent) {
    set_layout(new BoxLayout(Orientation::Vertical, Alignment::Middle, 0, 0));
    set_size(parent->size());
    set_fixed_size(parent->size());
    
    m_title_container = add<Widget>();
    m_title_container->set_layout(new BoxLayout(Orientation::Horizontal, Alignment::Middle, 0, 10));
    
    m_left_title_button_container = m_title_container->add<Widget>();
    m_left_title_button_container->set_layout(new BoxLayout(Orientation::Horizontal, Alignment::Middle, 0, 10));
    
    m_title_label = m_title_container->add<Label>(title);
    m_title_label->set_font_size(40);
    
    m_right_title_button_container = m_title_container->add<Widget>();
    m_right_title_button_container->set_layout(new BoxLayout(Orientation::Horizontal, Alignment::Middle, 0, 10));
    
    m_scroll = add<VScrollPanel>();
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
    button->set_selectable_color(Color(255, 255, 255, 200));
    button->set_callback([callback] {
        callback();
    });
    perform_layout();
}

void ContentWindow::set_right_title_button(int icon, const std::function<void()> &callback) {
    auto button = m_right_title_button_container->add<Button>("", icon);
    button->set_fixed_size(Size(60, 60));
    button->set_icon_extra_scale(3);
    button->set_selectable_color(Color(255, 255, 255, 200));
    button->set_callback([callback] {
        callback();
    });
    perform_layout();
}

void ContentWindow::perform_layout(NVGcontext *ctx) {
    int button_count = (int)m_left_title_button_container->children().size() + (int)m_right_title_button_container->children().size();
    int space = (2 + button_count) * 10;
    m_title_container->set_fixed_size(Size(width() - 40, 80));
    m_title_label->set_fixed_width(m_title_container->width() - button_count * 60 - space);
    m_scroll->set_fixed_size(Size(width() - 60, height() - 80));
    Widget::perform_layout(ctx);
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

static inline bool widget_intersect_target(Widget* widget, Widget* target, bool horizontal) {
    auto w_pos = widget->absolute_position();
    auto w_size = widget->size();
    auto t_pos = target->absolute_position();
    auto t_size = target->size();
    
    if (horizontal) {
        return w_pos.x() + w_size.x() >= t_pos.x() && w_pos.x() <= t_pos.x() + t_size.x();
    }
    return w_pos.y() + w_size.y() >= t_pos.y() && w_pos.y() <= t_pos.y() + t_size.y();
}

static inline int distance_between_widgets(Widget* widget, Widget* target) {
    return hypot(target->center().x() - widget->center().x(), target->center().y() - widget->center().y());
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
        
        if (left && widget_intersect_target(widget, target, false) && widget->center().x() < target->center().x()) {
            int new_distanse = target->center().x() - widget->center().x();
            if (new_distanse < distanse) {
                distanse = new_distanse;
                index = i;
            }
        } else if (right && widget_intersect_target(widget, target, false) && widget->center().x() > target->center().x()) {
            int new_distanse = widget->center().x() - target->center().x();
            if (new_distanse < distanse) {
                distanse = new_distanse;
                index = i;
            }
        } else if (up && widget_intersect_target(widget, target, true) && widget->center().y() < target->center().y()) {
            int new_distanse = target->center().y() - widget->center().y();
            if (new_distanse < distanse) {
                distanse = new_distanse;
                index = i;
            }
        } else if (down && widget_intersect_target(widget, target, true) && widget->center().y() > target->center().y()) {
            int new_distanse = widget->center().y() - target->center().y();
            if (new_distanse < distanse) {
                distanse = new_distanse;
                index = i;
            }
        }
    }
    
    if (index == -1) {
        distanse = std::numeric_limits<int>::max();
        
        for (int i = 0; i < widgets.size(); i++) {
             auto widget = widgets[i];
             
             if (widget == target) {
                 continue;
             }
             
             if (left && widget->center().x() < target->center().x()) {
                 int new_distanse = distance_between_widgets(target, widget);
                 if (new_distanse < distanse) {
                     distanse = new_distanse;
                     index = i;
                 }
             } else if (right && widget->center().x() > target->center().x()) {
                 int new_distanse = distance_between_widgets(widget, target);
                 if (new_distanse < distanse) {
                     distanse = new_distanse;
                     index = i;
                 }
             } else if (up && widget->center().y() < target->center().y()) {
                 int new_distanse = distance_between_widgets(target, widget);
                 if (new_distanse < distanse) {
                     distanse = new_distanse;
                     index = i;
                 }
             } else if (down && widget->center().y() > target->center().y()) {
                 int new_distanse = distance_between_widgets(widget, target);
                 if (new_distanse < distanse) {
                     distanse = new_distanse;
                     index = i;
                 }
             }
         }
    }
    
    if (index != -1) {
        return widgets[index];
    }
    return nullptr;
}

static inline Widget* find_new_selectable(std::vector<Widget *> selectables, int jid, int button, int action) {
    auto current = std::find_if(selectables.begin(), selectables.end(), [](auto c) { return c->selected(); });
     
     if (current != selectables.end() && (*current)->gamepad_button_event(jid, button, action)) {
         return NULL;
     }
     
     if (button >= NANOGUI_GAMEPAD_BUTTON_DPAD_UP && button <= NANOGUI_GAMEPAD_BUTTON_DPAD_LEFT) {
         if (current == selectables.end()) {
             selectables.front()->set_selected(true);
             return NULL;
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
             return new_selectable;
         }
     }
    return NULL;
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
    } else if (!messages.empty()) {
        find_new_selectable(selectables_child_recursive(messages.front()), jid, button, action);
    }
    
    if (!handle_button) {
        return false;
    }
    
    auto selectables = selectables_child_recursive(m_container);
    selectables.insert(selectables.end(), m_left_title_button_container->children().begin(), m_left_title_button_container->children().end());
    selectables.insert(selectables.end(), m_right_title_button_container->children().begin(), m_right_title_button_container->children().end());
    
    if (auto new_selectable = find_new_selectable(selectables, jid, button, action)) {
        auto pos = new_selectable->absolute_position().y() - container()->absolute_position().y();
        float offset = (pos + new_selectable->height() - m_scroll->height() / 1.5) / (float)(m_container->height() - m_scroll->height());
        m_scroll->set_scroll(fmin(fmax(offset, 0), 1));
    }
    return false;
}
