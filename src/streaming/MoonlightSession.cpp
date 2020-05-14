#include "MoonlightSession.hpp"
#include "GameStreamClient.hpp"
#include "Settings.hpp"
#include "Log.h"
#include <nanogui/nanogui.h>

static MoonlightSession* m_active_session = nullptr;

MoonlightSession::MoonlightSession(const std::string &address, int app_id) {
    m_address = address;
    m_app_id = app_id;
    
    m_active_session = this;
}

MoonlightSession::~MoonlightSession() {
    if (m_video_decoder) {
        delete m_video_decoder;
    }
    
    if (m_video_renderer) {
        delete m_video_renderer;
    }
    
    if (m_audio_renderer) {
        delete m_audio_renderer;
    }
    
    m_active_session = nullptr;
}

// MARK: Connection callbacks

static const char* stages[] = {
    "STAGE_NONE",
    "STAGE_PLATFORM_INIT",
    "STAGE_NAME_RESOLUTION",
    "STAGE_RTSP_HANDSHAKE",
    "STAGE_CONTROL_STREAM_INIT",
    "STAGE_VIDEO_STREAM_INIT",
    "STAGE_AUDIO_STREAM_INIT",
    "STAGE_INPUT_STREAM_INIT",
    "STAGE_CONTROL_STREAM_START",
    "STAGE_VIDEO_STREAM_START",
    "STAGE_AUDIO_STREAM_START",
    "STAGE_INPUT_STREAM_START"
};

void MoonlightSession::connection_stage_starting(int stage) {
    //LOG_FMT("Starting: %s\n", stages[stage]);
}

void MoonlightSession::connection_stage_complete(int stage) {
    //LOG_FMT("Complete: %s\n", stages[stage]);
}

void MoonlightSession::connection_stage_failed(int stage, int error_code) {
    //LOG_FMT("Failed: %s\n", stages[stage]);
}

void MoonlightSession::connection_started() {
    //LOG("Connection started\n");
}

void MoonlightSession::connection_terminated(int error_code) {
    //LOG("Connection terminated...\n");
    
    if (m_active_session) {
        m_active_session->m_is_active = false;
    }
}

void MoonlightSession::connection_log_message(const char* format, ...) {
    va_list list;
    va_start(list, format);
    vprintf(format, list);
    va_end(list);
}

void MoonlightSession::connection_rumble(unsigned short controller, unsigned short low_freq_motor, unsigned short high_freq_motor) {
    
}

void MoonlightSession::connection_status_update(int connection_status) {
    if (m_active_session) {
        m_active_session->m_connection_status_is_poor = connection_status == CONN_STATUS_POOR;
    }
}

// MARK: Video decoder callbacks

int MoonlightSession::video_decoder_setup(int video_format, int width, int height, int redraw_rate, void* context, int dr_flags) {
    if (m_active_session && m_active_session->m_video_decoder) {
        return m_active_session->m_video_decoder->setup(video_format, width, height, redraw_rate, context, dr_flags);
    }
    return DR_OK;
}

void MoonlightSession::video_decoder_start() {
    if (m_active_session && m_active_session->m_video_decoder) {
        m_active_session->m_video_decoder->start();
    }
}

void MoonlightSession::video_decoder_stop() {
    if (m_active_session && m_active_session->m_video_decoder) {
        m_active_session->m_video_decoder->stop();
    }
}

void MoonlightSession::video_decoder_cleanup() {
    if (m_active_session && m_active_session->m_video_decoder) {
        m_active_session->m_video_decoder->cleanup();
    }
}

int MoonlightSession::video_decoder_submit_decode_unit(PDECODE_UNIT decode_unit) {
    if (m_active_session && m_active_session->m_video_decoder) {
        return m_active_session->m_video_decoder->submit_decode_unit(decode_unit);
    }
    return DR_OK;
}

// MARK: Audio callbacks

int MoonlightSession::audio_renderer_init(int audio_configuration, const POPUS_MULTISTREAM_CONFIGURATION opus_config, void* context, int ar_flags) {
    if (m_active_session && m_active_session->m_audio_renderer) {
        return m_active_session->m_audio_renderer->init(audio_configuration, opus_config, context, ar_flags);
    }
    return DR_OK;
}

void MoonlightSession::audio_renderer_start() {
    if (m_active_session && m_active_session->m_audio_renderer) {
        m_active_session->m_audio_renderer->start();
    }
}

void MoonlightSession::audio_renderer_stop() {
    if (m_active_session && m_active_session->m_audio_renderer) {
        m_active_session->m_audio_renderer->stop();
    }
}

void MoonlightSession::audio_renderer_cleanup() {
    if (m_active_session && m_active_session->m_audio_renderer) {
        m_active_session->m_audio_renderer->cleanup();
    }
}

void MoonlightSession::audio_renderer_decode_and_play_sample(char* sample_data, int sample_length) {
    if (m_active_session && m_active_session->m_audio_renderer) {
        m_active_session->m_audio_renderer->decode_and_play_sample(sample_data, sample_length);
    }
}

// MARK: MoonlightSession

void MoonlightSession::start(std::function<void(bool)> callback) {
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
    
    switch (Settings::settings()->video_codec()) {
        case H264:
            m_config.supportsHevc = 0;
            break;
        case H265:
            m_config.supportsHevc = 1;
            m_config.hevcBitratePercentageMultiplier = 75;
            break;
        default:
            break;
    }
    
    LiInitializeConnectionCallbacks(&m_connection_callbacks);
    m_connection_callbacks.stageStarting = connection_stage_starting;
    m_connection_callbacks.stageComplete = connection_stage_complete;
    m_connection_callbacks.stageFailed = connection_stage_failed;
    m_connection_callbacks.connectionStarted = connection_started;
    m_connection_callbacks.connectionTerminated = connection_terminated;
    m_connection_callbacks.logMessage = connection_log_message;
    m_connection_callbacks.rumble = connection_rumble;
    m_connection_callbacks.connectionStatusUpdate = connection_status_update;
    
    LiInitializeVideoCallbacks(&m_video_callbacks);
    m_video_callbacks.setup = video_decoder_setup;
    m_video_callbacks.start = video_decoder_start;
    m_video_callbacks.stop = video_decoder_stop;
    m_video_callbacks.cleanup = video_decoder_cleanup;
    m_video_callbacks.submitDecodeUnit = video_decoder_submit_decode_unit;
    
    if (m_video_decoder) {
        m_video_callbacks.capabilities = m_video_decoder->capabilities();
    }
    
    LiInitializeAudioCallbacks(&m_audio_callbacks);
    m_audio_callbacks.init = audio_renderer_init;
    m_audio_callbacks.start = audio_renderer_start;
    m_audio_callbacks.stop = audio_renderer_stop;
    m_audio_callbacks.cleanup = audio_renderer_cleanup;
    m_audio_callbacks.decodeAndPlaySample = audio_renderer_decode_and_play_sample;
    
    if (m_audio_renderer) {
        m_audio_callbacks.capabilities = m_audio_renderer->capabilities();
    }
    
    GameStreamClient::client()->start(m_address, m_config, m_app_id, [this, callback](auto result) {
        if (result.isSuccess()) {
            m_config = result.value();
            
            perform_async([this, callback] {
                auto m_data = GameStreamClient::client()->server_data(m_address);
                LiStartConnection(&m_data.serverInfo, &m_config, &m_connection_callbacks, &m_video_callbacks, &m_audio_callbacks, NULL, 0, NULL, 0);
                
                nanogui::async([this, callback] {
                    callback(true);
                });
            });
        } else {
            LOG_FMT("Failed to start stream: %s\n", result.error().c_str());
            callback(false);
        }
    });
}

void MoonlightSession::stop(int terminate_app) {
    if (terminate_app) {
        GameStreamClient::client()->quit(m_address, [](auto _) {});
    }
    
    LiStopConnection();
}

void MoonlightSession::draw() {
    if (m_video_decoder && m_video_renderer) {
        if (auto frame = m_video_decoder->frame()) {
            m_video_renderer->draw(m_config.width, m_config.height, frame);
        }
        
        m_session_stats.video_decode_stats = *m_video_decoder->video_decode_stats();
        m_session_stats.video_render_stats = *m_video_renderer->video_render_stats();
    }
}
