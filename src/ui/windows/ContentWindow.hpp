#include <nanogui/nanogui.h>
#include "Application.hpp"
#pragma once

class ContentWindow: public nanogui::Widget {
public:
    ContentWindow(Widget *parent, const std::string& title);
    
    virtual void window_appear() {};
    virtual void window_disappear() {};
    
    void draw(NVGcontext *ctx) override;
    
    void set_left_title_button(int icon, const std::function<void()> &callback);
    void set_right_title_button(int icon, const std::function<void()> &callback);
    
    void set_left_pop_button() {
        set_left_title_button(FA_CHEVRON_LEFT, [this]() {
            this->pop();
        });
    }
    
    void clean_right_title_buttons() {
        auto children = m_right_title_button_container->children();
        for (auto child: children) {
            if (m_right_title_button_container->child_index(child) != -1) {
                m_right_title_button_container->remove_child(child);
            }
        }
        perform_layout();
    }
    
    nanogui::Widget* container() const {
        return m_container;
    }
    
    void clean_container() {
        auto children = m_container->children();
        for (auto child: children) {
            if (m_container->child_index(child) != -1) {
                m_container->remove_child(child);
            }
        }
        perform_layout();
    }
    
    template<typename WidgetClass, typename... Args>
    void push(const Args&... args) {
        application()->push_widget(new WidgetClass(screen(), args...));
    }
    
    void pop() {
        application()->pop_window();
    }
    
    void perform_layout(NVGcontext *ctx) override;
    
    void perform_layout() {
        application()->perform_layout();
        application()->update_focus(NULL);
    }
    
    void set_box_layout(nanogui::Orientation orientation, nanogui::Alignment alignment = nanogui::Alignment::Middle, int margin = 30, int spacing = 10) {
        m_container->set_layout(new nanogui::BoxLayout(orientation, alignment, margin, spacing));
    }
    
    bool gamepad_button_event(int jid, int button, int action) override;
    
private:
    Application* application() {
        auto application = static_cast<Application *>(screen());
        return application;
    }
    
    nanogui::Widget* m_title_container;
    nanogui::Widget* m_left_title_button_container;
    nanogui::Widget* m_right_title_button_container;
    nanogui::Label* m_title_label;
    nanogui::VScrollPanel* m_scroll;
    nanogui::Widget* m_container;
};
