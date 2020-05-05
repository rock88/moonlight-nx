#include <nanogui/nanogui.h>
#include "GameStreamClient.hpp"
#pragma once

class HostButton: public nanogui::Button {
public:
    HostButton(Widget* parent, const std::string &host);
    
    std::string host() const {
        return m_host;
    }
    
    bool is_active() const {
        return m_is_active;
    }
    
    bool is_paired() const {
        return m_is_paired;
    }
    
    void draw(NVGcontext *ctx) override;
    
private:
    std::string m_host;
    int m_host_status_icon;
    bool m_is_active = false;
    bool m_is_paired = false;
};
