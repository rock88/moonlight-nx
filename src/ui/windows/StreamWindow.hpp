#include "ContentWindow.hpp"
#include "Server.hpp"
#pragma once

class StreamWindow: public nanogui::Widget {
public:
    StreamWindow(Widget *parent, SERVER_DATA data, int id);
    
    void setup_stream();
    
    void draw(NVGcontext *ctx) override;
    
    bool mouse_button_event(const nanogui::Vector2i &p, int button, bool down, int modifiers) override;
    bool mouse_motion_event(const nanogui::Vector2i &p, const nanogui::Vector2i &rel, int button, int modifiers) override;
    
private:
    SERVER_DATA m_data;
    STREAM_CONFIGURATION m_config;
    int m_id;
};
