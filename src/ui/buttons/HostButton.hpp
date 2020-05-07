#include <nanogui/nanogui.h>
#include "GameStreamClient.hpp"
#pragma once

class HostButton: public nanogui::Button {
public:
    HostButton(Widget* parent, const std::string &address);
    
    std::string address() const {
        return m_address;
    }
    
    bool is_active() const {
        return m_is_active;
    }
    
    bool is_paired() const {
        return m_is_paired;
    }
    
    void draw(NVGcontext *ctx) override;
    
private:
    std::string m_address;
    nanogui::Label* m_label;
    int m_host_status_icon;
    bool m_is_active = false;
    bool m_is_paired = false;
};
