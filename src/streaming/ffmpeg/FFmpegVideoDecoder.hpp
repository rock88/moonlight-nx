#include "IFFmpegVideoDecoder.hpp"
#include <pthread.h>
#pragma once

class IFFmpegHardwareVideoDecoder {
public:
    virtual ~IFFmpegHardwareVideoDecoder() {};
    virtual bool prepare_decoder_context(AVCodecContext* context, AVDictionary** options) = 0;
};

class FFmpegVideoDecoder: public IFFmpegVideoDecoder {
public:
    FFmpegVideoDecoder();
    ~FFmpegVideoDecoder();
    
    void set_hardware_video_decoder(IFFmpegHardwareVideoDecoder* hardware_video_decoder) {
        m_hardware_video_decoder = hardware_video_decoder;
    }
    
    int setup(int video_format, int width, int height, int redraw_rate, void *context, int dr_flags) override;
    void cleanup() override;
    int submit_decode_unit(PDECODE_UNIT decode_unit) override;
    int capabilities() const override;
    AVFrame* frame() const override;
    VideoDecodeStats* video_decode_stats() override;
    
private:
    int decode(char* indata, int inlen);
    AVFrame* get_frame(bool native_frame);
    
    IFFmpegHardwareVideoDecoder* m_hardware_video_decoder = nullptr;
    
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
    AVFrame* m_frame = nullptr;
    pthread_mutex_t m_mutex;
};
