#include "ContentWindow.hpp"
#include "GameStreamClient.hpp"
#include "LoadingOverlay.hpp"
#pragma once

class StreamWindow: public nanogui::Widget {
public:
    StreamWindow(Widget *parent, const std::string &address, int app_id);
    
    void setup_stream();
    
    void draw(NVGcontext *ctx) override;
    
    bool mouse_button_event(const nanogui::Vector2i &p, int button, bool down, int modifiers) override;
    bool mouse_motion_event(const nanogui::Vector2i &p, const nanogui::Vector2i &rel, int button, int modifiers) override;
    
    void terminate();
    
private:
    std::string m_address;
    int m_app_id;
    STREAM_CONFIGURATION m_config;
    LoadingOverlay* m_loader;
    bool m_connection_status_is_poor;
};
