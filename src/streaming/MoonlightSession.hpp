#include "GameStreamClient.hpp"
#include "IAudioRenderer.hpp"
#include "IVideoRenderer.hpp"
#include "IFFmpegVideoDecoder.hpp"
#pragma once

struct SessionStats {
    VideoDecodeStats video_decode_stats;
    VideoRenderStats video_render_stats;
};

class MoonlightSession {
public:
    MoonlightSession(const std::string &address, int app_id);
    ~MoonlightSession();

    void set_video_decoder(IFFmpegVideoDecoder* video_decoder) {
        m_video_decoder = video_decoder;
    }
    
    void set_video_renderer(IVideoRenderer* video_renderer) {
        m_video_renderer = video_renderer;
    }
    
    void set_audio_renderer(IAudioRenderer* audio_renderer) {
        m_audio_renderer = audio_renderer;
    }
    
    void start(ServerCallback<bool> callback);
    void stop(int terminate_app);
    
    void draw();
    
    bool is_active() const {
        return m_is_active;
    }
    
    bool connection_status_is_poor() const {
        return m_connection_status_is_poor;
    }
    
    SessionStats* session_stats() const {
        return (SessionStats*)&m_session_stats;
    }
    
private:
    static void connection_stage_starting(int);
    static void connection_stage_complete(int);
    static void connection_stage_failed(int, int);
    static void connection_started();
    static void connection_terminated(int);
    static void connection_log_message(const char* format, ...);
    static void connection_rumble(unsigned short, unsigned short, unsigned short);
    static void connection_status_update(int);

    static int video_decoder_setup(int, int, int, int, void*, int);
    static void video_decoder_start();
    static void video_decoder_stop();
    static void video_decoder_cleanup();
    static int video_decoder_submit_decode_unit(PDECODE_UNIT);
    
    static int audio_renderer_init(int, const POPUS_MULTISTREAM_CONFIGURATION, void*, int);
    static void audio_renderer_start();
    static void audio_renderer_stop();
    static void audio_renderer_cleanup();
    static void audio_renderer_decode_and_play_sample(char*, int);
    
    std::string m_address;
    int m_app_id;
    STREAM_CONFIGURATION m_config;
    CONNECTION_LISTENER_CALLBACKS m_connection_callbacks;
    DECODER_RENDERER_CALLBACKS m_video_callbacks;
    AUDIO_RENDERER_CALLBACKS m_audio_callbacks;
    
    IFFmpegVideoDecoder* m_video_decoder = nullptr;
    IVideoRenderer* m_video_renderer = nullptr;
    IAudioRenderer* m_audio_renderer = nullptr;
    
    bool m_is_active = true;
    bool m_connection_status_is_poor = false;
    
    SessionStats m_session_stats = {};
};
