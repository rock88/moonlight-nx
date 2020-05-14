#include "ContentWindow.hpp"
#include "GameStreamClient.hpp"
#include "LoadingOverlay.hpp"
#include "MoonlightSession.hpp"
#pragma once

class StreamWindow: public nanogui::Widget {
public:
    StreamWindow(Widget *parent, const std::string &address, int app_id);
    ~StreamWindow();
    
    void draw(NVGcontext *ctx) override;
    
    bool mouse_button_event(const nanogui::Vector2i &p, int button, bool down, int modifiers) override;
    bool mouse_motion_event(const nanogui::Vector2i &p, const nanogui::Vector2i &rel, int button, int modifiers) override;
    
    void terminate(bool close_app);
    
private:
    MoonlightSession* m_session;
    LoadingOverlay* m_loader;
    bool m_draw_stats = false;
};
