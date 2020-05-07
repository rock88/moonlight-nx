#include "StreamWindow.hpp"
#include "LoadingOverlay.hpp"
#include "GameStreamClient.hpp"
#include "Settings.hpp"
#include "InputController.hpp"
#include "gl_render.h"
#include "video_decoder.h"
#include "audio_decoder.h"
#include "nanovg.h"
#include "libretro.h"
#include <algorithm>
#include <memory>

using namespace nanogui;

static std::weak_ptr<StreamWindow *> _weak;

StreamWindow::StreamWindow(Widget *parent, const std::string &address, int app_id): Widget(parent) {
    _weak = std::make_shared<StreamWindow *>(this);
    
    m_address = address;
    m_app_id = app_id;
    m_connection_status_is_poor = false;
    m_size = parent->size();
    
    LiInitializeStreamConfiguration(&m_config);
    
    int h = Settings::settings()->resolution();
    int w = h * 16 / 9;
    m_config.width = w;
    m_config.height = h;
    m_config.fps = Settings::settings()->fps();
    m_config.audioConfiguration = AUDIO_CONFIGURATION_STEREO;
    m_config.packetSize = 1392;
    m_config.streamingRemotely = STREAM_CFG_LOCAL;
    m_config.bitrate = Settings::settings()->bitrate();
    
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
        CONNECTION_LISTENER_CALLBACKS connection_listener_callbacks = {
            .stageStarting = NULL,
            .stageComplete = NULL,
            .stageFailed = NULL,
            .connectionStarted = NULL,
            .connectionTerminated = [](int errorCode) {
                if (auto stream = _weak.lock()) {
                    (*stream)->terminate(true);
                }
            },
            .logMessage = NULL,
            .rumble = NULL,
            .connectionStatusUpdate = [](int status) {
                if (auto stream = _weak.lock()) {
                    if (status == CONN_STATUS_POOR) {
                        (*stream)->m_connection_status_is_poor = true;
                    } else {
                        (*stream)->m_connection_status_is_poor = false;
                    }
                }
            }
        };
        
        auto m_data = GameStreamClient::client()->server_data(m_address);
        LiStartConnection(&m_data.serverInfo, &m_config, &connection_listener_callbacks, &video_decoder_callbacks, &audio_decoder_callbacks, NULL, 0, NULL, 0);
        
        async([this] {
            if (m_loader) {
                m_loader->dispose();
                m_loader = NULL;
            }
        });
    });
}

void StreamWindow::draw(NVGcontext *ctx) {
    nvgSave(ctx);
    
    gl_render_setup(m_config.width, m_config.height);
    
    if (frame != NULL) {
        gl_render_draw(frame->data);
    }
    
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
    
    if (close_app) {
        GameStreamClient::client()->quit(m_address, [](auto _) {});
    }
    
    LiStopConnection();
    
    if (auto app = dynamic_cast<Application *>(screen())) {
        app->pop_window();
    }
}
