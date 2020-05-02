#include "StreamWindow.hpp"
#include "LoadingOverlay.hpp"
#include "Server.hpp"
#include "gl_render.h"
#include "video_decoder.h"

using namespace nanogui;

StreamWindow::StreamWindow(Widget *parent, SERVER_DATA data, int id): nanogui::Widget(parent) {
    m_data = data;
    m_id = id;
    
    int h = 720;
    int w = h * 16 / 9;
    m_config.width = w;
    m_config.height = h;
    m_config.fps = 30;
    m_config.audioConfiguration = AUDIO_CONFIGURATION_STEREO;
    m_config.packetSize = 1392;
    m_config.streamingRemotely = 2;
    m_config.bitrate = 1000;
    
    auto loader = add<LoadingOverlay>();
    
    Server::server()->start(m_data, m_config, m_id, [this, loader](auto result) {
        loader->dispose();
        
        if (result.isSuccess()) {
            m_config = result.value();
            setup_stream();
        } else {
            screen()->add<MessageDialog>(MessageDialog::Type::Information, "Error", result.error());
            
            auto app = static_cast<Application *>(screen());
            app->pop_window();
        }
    });
}

void StreamWindow::setup_stream() {
    LiStartConnection(&m_data.serverInfo, &m_config, NULL, &video_decoder_callbacks, NULL, NULL, 0, NULL, 0);
}

void StreamWindow::draw(NVGcontext *ctx) {
    gl_render_setup(1280, 720);
    
    if (frame != NULL) {
        gl_render_draw((char **)frame->data);
    }
}

bool StreamWindow::mouse_button_event(const nanogui::Vector2i &p, int button, bool down, int modifiers) {
    if (button == 0) {
        LiSendMouseButtonEvent(down ? BUTTON_ACTION_PRESS : BUTTON_ACTION_RELEASE, BUTTON_LEFT);
    }
    
    return true;
}

bool StreamWindow::mouse_motion_event(const Vector2i &p, const Vector2i &rel, int button, int modifiers) {
    LiSendMousePositionEvent(p.x(), p.y(), width(), height());
    return false;
}
