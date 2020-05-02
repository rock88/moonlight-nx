#include <nanogui/nanogui.h>
#include "Server.hpp"
#pragma once

class HostButton: public nanogui::Button {
public:
    HostButton(Widget* parent, const std::string &host);
    
    SERVER_DATA server_data() {
        return m_data;
    }
    
    void draw(NVGcontext *ctx) override;
    
private:
    std::string m_host;
    SERVER_DATA m_data;
};
