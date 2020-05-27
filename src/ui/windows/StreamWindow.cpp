#include "StreamWindow.hpp"
#include "LoadingOverlay.hpp"
#include "InputController.hpp"
#include "FFmpegVideoDecoder.hpp"
#include "GLVideoRenderer.hpp"
#ifdef __SWITCH__
#include "AudrenAudioRenderer.hpp"
#endif
#include "DebugFileRecorderAudioRenderer.hpp"
#include "nanovg.h"
#include <algorithm>
#include <memory>

using namespace nanogui;

StreamWindow::StreamWindow(Widget *parent, const std::string &address, int app_id): Widget(parent) {
    m_size = parent->size();
    m_session = new MoonlightSession(address, app_id);
    
    m_session->set_video_decoder(new FFmpegVideoDecoder());
    m_session->set_video_renderer(new GLVideoRenderer());
    
    #ifdef __SWITCH__
    m_session->set_audio_renderer(new AudrenAudioRenderer());
    #else
    m_session->set_audio_renderer(new DebugFileRecorderAudioRenderer(false));
    #endif
    
    m_loader = add<LoadingOverlay>();
    
    inc_ref();
    m_session->start([this](auto result) {
        if (m_loader) {
            m_loader->dispose();
            m_loader = NULL;
        }
        
        if (result.isSuccess()) {
            //
        } else {
            screen()->add<MessageDialog>(MessageDialog::Type::Information, "Error", result.error());
            
            auto app = static_cast<Application *>(screen());
            app->pop_window();
        }
        
        dec_ref();
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
    
    if (m_session->connection_status_is_poor()) {
        nvgFillColor(ctx, Color(255, 255, 255, 200));
        nvgFontSize(ctx, 20);
        nvgFontFace(ctx, "icons");
        nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
        nvgText(ctx, 20, height() - 30, utf8(FA_EXCLAMATION_TRIANGLE).data(), NULL);
        nvgFontFace(ctx, "sans-bold");
        nvgText(ctx, 50, height() - 28, "Bad connection...", NULL);
    }
    
    if (m_draw_stats) {
        static char output[1024];
        
        int offset = 0;
        
        auto stats = m_session->session_stats();
        
        offset += sprintf(&output[offset],
                          "Estimated host PC frame rate: %.2f FPS\n"
                          "Incoming frame rate from network: %.2f FPS\n"
                          "Decoding frame rate: %.2f FPS\n"
                          "Rendering frame rate: %.2f FPS\n",
                          stats->video_decode_stats.total_fps,
                          stats->video_decode_stats.received_fps,
                          stats->video_decode_stats.decoded_fps,
                          stats->video_render_stats.rendered_fps);
        
        offset += sprintf(&output[offset],
                          "Frames dropped by your network connection: %.2f%% (Total: %u)\n"
                          "Average receive time: %.2f ms\n"
                          "Average decoding time: %.2f ms\n"
                          "Average rendering time: %.2f ms\n",
                          (float)stats->video_decode_stats.network_dropped_frames / stats->video_decode_stats.total_frames * 100,
                          stats->video_decode_stats.network_dropped_frames,
                          (float)stats->video_decode_stats.total_reassembly_time / stats->video_decode_stats.received_frames,
                          (float)stats->video_decode_stats.total_decode_time / stats->video_decode_stats.decoded_frames,
                          (float)stats->video_render_stats.total_render_time / stats->video_render_stats.rendered_frames);
        
        nvgFillColor(ctx, Color(0, 255, 0, 255));
        nvgFontFace(ctx, "sans-bold");
        nvgFontSize(ctx, 20);
        nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_BOTTOM);
        nvgTextBox(ctx, 20, 30, width(), output, NULL);
    }
    
    // TODO: Get out of here...
    if (GAME_PAD_COMBO(DOWN_FLAG)) {
        async([this] { this->terminate(true); });
    } else if (GAME_PAD_COMBO(UP_FLAG)) {
        async([this] { this->terminate(false); });
    }
    
    if (!m_draw_stats && GAME_PAD_COMBO_R(LEFT_FLAG)) {
        m_draw_stats = true;
    } else if (m_draw_stats && GAME_PAD_COMBO_R(RIGHT_FLAG)) {
        m_draw_stats = false;
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
    InputController::controller()->stop_rumple();
    
    if (m_loader) {
        m_loader->dispose();
        m_loader = NULL;
    }
    
    m_session->stop(close_app);
    
    if (auto app = dynamic_cast<Application *>(screen())) {
        app->pop_window();
    }
}
