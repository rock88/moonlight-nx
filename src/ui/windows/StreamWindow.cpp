#include "StreamWindow.hpp"
#include "LoadingOverlay.hpp"
#include "InputController.hpp"
#include "FFmpegVideoDecoder.hpp"
#include "GLVideoRenderer.hpp"
#include "RetroAudioRenderer.hpp"
#include "nanovg.h"
#include <algorithm>
#include <memory>

using namespace nanogui;

StreamWindow::StreamWindow(Widget *parent, const std::string &address, int app_id): Widget(parent) {
    m_size = parent->size();
    m_session = new MoonlightSession(address, app_id);
    
    m_session->set_video_decoder(new FFmpegVideoDecoder());
    m_session->set_video_renderer(new GLVideoRenderer());
    
    #ifdef __LIBRETRO__
    m_session->set_audio_renderer(new RetroAudioRenderer());
    #endif
    
    m_loader = add<LoadingOverlay>();
    
    m_session->start([this](auto result) {
        if (result) {
            if (m_loader) {
                m_loader->dispose();
                m_loader = NULL;
            }
        } else {
            auto app = static_cast<Application *>(screen());
            app->pop_window();
        }
    });
}

StreamWindow::~StreamWindow() {
    delete m_session;
}

void StreamWindow::draw(NVGcontext *ctx) {
    if (!m_session->is_active()) {
        async([this] { this->terminate(false); });
    }
    
    nvgSave(ctx);
    
    m_session->draw();
    
    nvgRestore(ctx);
    
    if (m_connection_status_is_poor) {
        nvgFillColor(ctx, Color(255, 255, 255, 200));
        nvgFontSize(ctx, 20);
        nvgFontFace(ctx, "icons");
        nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
        nvgText(ctx, 20, height() - 30, utf8(FA_EXCLAMATION_TRIANGLE).data(), NULL);
        nvgFontFace(ctx, "sans-bold");
        nvgText(ctx, 50, height() - 28, "Bad connection...", NULL);
    }
    
    // TODO: Get out of here...
    if (GAME_PAD_COMBO(DOWN_FLAG)) {
        async([this] { this->terminate(true); });
    } else if (GAME_PAD_COMBO(UP_FLAG)) {
        async([this] { this->terminate(false); });
    }
    
    InputController::controller()->send_to_stream();
}

bool StreamWindow::mouse_button_event(const nanogui::Vector2i &p, int button, bool down, int modifiers) {
    return true;
}

bool StreamWindow::mouse_motion_event(const Vector2i &p, const Vector2i &rel, int button, int modifiers) {
    return true;
}

void StreamWindow::terminate(bool close_app) {
    if (m_loader) {
        m_loader->dispose();
        m_loader = NULL;
    }
    
    m_session->stop(close_app);
    
    if (auto app = dynamic_cast<Application *>(screen())) {
        app->pop_window();
    }
}
