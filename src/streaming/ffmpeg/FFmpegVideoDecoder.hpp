#include "IFFmpegVideoDecoder.hpp"
#include <condition_variable>
#pragma once

class semaphore {
private:
    std::mutex m_mutex;
    std::condition_variable m_condition;
    unsigned long m_count = 0; // Initialized as locked.
    
public:
    void notify() {
        std::lock_guard<std::mutex> lock(m_mutex);
        ++m_count;
        m_condition.notify_one();
    }
    
    void wait() {
        std::unique_lock<std::mutex> lock(m_mutex);
        while (!m_count) { // Handle spurious wake-ups.
            m_condition.wait(lock);
        }
        --m_count;
    }
    
    bool is_waiting() const {
        return m_count == 0;
    }
};

class FFmpegVideoDecoder: public IFFmpegVideoDecoder {
public:
    FFmpegVideoDecoder(IAVFrameSyncDrawer* drawer);
    ~FFmpegVideoDecoder();
    
    int setup(int video_format, int width, int height, int redraw_rate, void *context, int dr_flags) override;
    void stop() override;
    void cleanup() override;
    int submit_decode_unit(PDECODE_UNIT decode_unit) override;
    int capabilities() const override;
    VideoDecodeStats* video_decode_stats() override;
    
private:
    int decode(char* indata, int inlen);
    AVFrame* get_frame(bool native_frame);
    
    AVPacket m_packet;
    AVCodec* m_decoder = nullptr;
    AVCodecContext* m_decoder_context = nullptr;
    AVFrame** m_frames = nullptr;
    
    int m_stream_fps = 0;
    int m_frames_in = 0;
    int m_frames_out = 0;
    int m_frames_count = 0;
    int m_current_frame = 0, m_next_frame = 0;
    uint32_t m_last_frame = 0;
    
    VideoDecodeStats m_video_decode_stats = {};
    
    char* m_ffmpeg_buffer = nullptr;
    semaphore m_semaphore;
    bool m_is_started = false;
};
