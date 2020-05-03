#include "StreamWindow.hpp"
#include "LoadingOverlay.hpp"
#include "GameStreamClient.hpp"
#include "gl_render.h"
#include "video_decoder.h"
#include "nanovg.h"

using namespace nanogui;

StreamWindow::StreamWindow(Widget *parent, const std::string &address, int app_id): Widget(parent) {
    m_address = address;
    m_app_id = app_id;
    
    LiInitializeStreamConfiguration(&m_config);
    
    int h = 720;
    int w = h * 16 / 9;
    m_config.width = w;
    m_config.height = h;
    m_config.fps = 30;
    m_config.audioConfiguration = AUDIO_CONFIGURATION_STEREO;
    m_config.packetSize = 1392;
    m_config.streamingRemotely = 2;
    m_config.bitrate = 2000;
    
    m_loader = add<LoadingOverlay>();
    
    GameStreamClient::client()->start(m_address, m_config, m_app_id, [this](auto result) {
        if (result.isSuccess()) {
            m_config = result.value();
            setup_stream();
        } else {
            m_loader->dispose();
            screen()->add<MessageDialog>(MessageDialog::Type::Information, "Error", result.error());
            
            auto app = static_cast<Application *>(screen());
            app->pop_window();
        }
    });
}

void StreamWindow::setup_stream() {
    perform_async([this] {
        auto m_data = GameStreamClient::client()->server_data(m_address);
        LiStartConnection(&m_data.serverInfo, &m_config, NULL, &video_decoder_callbacks, NULL, NULL, 0, NULL, 0);
        
        async([this] {
            m_loader->dispose();
        });
    });

}

void StreamWindow::draw(NVGcontext *ctx) {
    nvgSave(ctx);
    
    gl_render_setup(1280, 720);

    if (frame != NULL) {
        gl_render_draw(frame->data);
    }
    
    nvgRestore(ctx);
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
