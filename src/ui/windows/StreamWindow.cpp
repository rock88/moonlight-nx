#include "StreamWindow.hpp"
#include "LoadingOverlay.hpp"
#include "GameStreamClient.hpp"
#include "gl_render.h"
#include "video_decoder.h"
#include "audio_decoder.h"
#include "nanovg.h"
#include "moonlight_libretro_wrapper.h"
#include "libretro.h"
#include <algorithm>
#include <memory>

static bool pressed = false, sent_l_press = false, sent_r_press = false;
static int current_mouse_x = 0, current_mouse_y = 0;
static int start_mouse_x = 0, start_mouse_y = 0;

using namespace nanogui;

static std::weak_ptr<StreamWindow *> _weak;

StreamWindow::StreamWindow(Widget *parent, const std::string &address, int app_id): Widget(parent) {
    _weak = std::make_shared<StreamWindow *>(this);
    
    m_address = address;
    m_app_id = app_id;
    m_connection_status_is_poor = false;
    m_size = parent->size();
    
    pressed = sent_l_press = sent_r_press = false;
    current_mouse_x = width() / 2;
    current_mouse_y = height() / 2;
    start_mouse_x = start_mouse_y = 0;
    
    LiInitializeStreamConfiguration(&m_config);
    
    int h = 720;
    int w = h * 16 / 9;
    m_config.width = w;
    m_config.height = h;
    m_config.fps = 30;
    m_config.audioConfiguration = AUDIO_CONFIGURATION_STEREO;
    m_config.packetSize = 1392;
    m_config.streamingRemotely = 2;
    m_config.bitrate = 1000;
    
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
                    (*stream)->terminate();
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
    
    gl_render_setup(width(), height());
    
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
    if (keyboard_state[RETROK_q] || ((game_pad_state.buttonFlags & LB_FLAG) && (game_pad_state.buttonFlags & RB_FLAG) && (game_pad_state.buttonFlags & DOWN_FLAG))) {
        async([this] {
            this->terminate();
        });
    }
    
    LiSendControllerEvent(
        game_pad_state.buttonFlags,
        game_pad_state.leftTrigger,
        game_pad_state.rightTrigger,
        game_pad_state.leftStickX,
        game_pad_state.leftStickY,
        game_pad_state.rightStickX,
        game_pad_state.rightStickY
    );
}

bool StreamWindow::mouse_button_event(const nanogui::Vector2i &p, int button, bool down, int modifiers) {
#if defined(__LIBRETRO__) && (defined(__LAKKA_SWITCH__) || defined(__APPLE__))
    if (button == 0) {
        if (down && !pressed) {
            start_mouse_x = p.x();
            start_mouse_y = p.y();
        }
        pressed = down;
        
        if (pressed && keyboard_state[RETROK_LCTRL]) {
            sent_l_press = true;
            LiSendMouseButtonEvent(BUTTON_ACTION_PRESS, BUTTON_LEFT);
        } else if (sent_l_press) {
            sent_l_press = false;
            LiSendMouseButtonEvent(BUTTON_ACTION_RELEASE, BUTTON_LEFT);
        }
        
        if (pressed && keyboard_state[RETROK_LALT]) {
            sent_r_press = true;
            LiSendMouseButtonEvent(BUTTON_ACTION_PRESS, BUTTON_RIGHT);
        } else if (sent_r_press) {
            sent_r_press = false;
            LiSendMouseButtonEvent(BUTTON_ACTION_RELEASE, BUTTON_RIGHT);
        }
    }
#else
    if (button == 0) {
        LiSendMouseButtonEvent(down ? BUTTON_ACTION_PRESS : BUTTON_ACTION_RELEASE, BUTTON_LEFT);
    } else if (button == 1) {
        LiSendMouseButtonEvent(down ? BUTTON_ACTION_PRESS : BUTTON_ACTION_RELEASE, BUTTON_RIGHT);
    }
#endif
    return true;
}

bool StreamWindow::mouse_motion_event(const Vector2i &p, const Vector2i &rel, int button, int modifiers) {
#if defined(__LIBRETRO__) && (defined(__LAKKA_SWITCH__) || defined(__APPLE__))
    if (pressed) {
        current_mouse_x = std::min(std::max(current_mouse_x + p.x() - start_mouse_x, 0), width());
        current_mouse_y = std::min(std::max(current_mouse_y + p.y() - start_mouse_y, 0), height());
        
        LiSendMousePositionEvent(current_mouse_x, current_mouse_y, width(), height());
        
        start_mouse_x = p.x();
        start_mouse_y = p.y();
    }
#else
    LiSendMousePositionEvent(p.x(), p.y(), width(), height());
#endif
    return true;
}

void StreamWindow::terminate() {
    if (m_loader) {
        m_loader->dispose();
        m_loader = NULL;
    }
    
    GameStreamClient::client()->quit(m_address, [](auto _) {});
    LiStopConnection();
    
    auto app = static_cast<Application *>(screen());
    app->pop_window();
}
