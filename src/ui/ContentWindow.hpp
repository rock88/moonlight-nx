#include <nanogui/nanogui.h>
#pragma once

#define Size(x, y) (nanogui::Vector2f((x), (y)))

class ContentWindow: public nanogui::Widget {
public:
    ContentWindow(Widget *parent, const std::string& title);
    
    void draw(NVGcontext *ctx) override;
    
    void set_left_title_button(int icon, const std::function<void()> &callback);
    void pop();
    
    void set_left_pop_button() {
        set_left_title_button(FA_ANGLE_LEFT, [this]() {
            this->pop();
        });
    }
    
    nanogui::Widget* container() const {
        return m_container;
    }
    
    void set_box_layout(nanogui::Orientation orientation, nanogui::Alignment alignment = nanogui::Alignment::Middle, int margin = 0, int spacing = 0) {
        m_container->set_layout(new nanogui::BoxLayout(orientation, alignment, margin, spacing));
    }
    
    void set_group_layout() {
        m_container->set_layout(new nanogui::GroupLayout());
    }
    
private:
    nanogui::Widget* m_title_button_container;
    nanogui::Label* m_title_label;
    nanogui::VScrollPanel* m_scroll;
    nanogui::Widget* m_container;
};
