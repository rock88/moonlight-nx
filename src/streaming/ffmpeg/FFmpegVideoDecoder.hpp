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
    FFmpegVideoDecoder(IFFmpegHardwareVideoDecoder* hardware_video_decoder = nullptr);
    ~FFmpegVideoDecoder();
    
    int setup(int video_format, int width, int height, int redraw_rate, void *context, int dr_flags) override;
    void cleanup() override;
    int submit_decode_unit(PDECODE_UNIT decode_unit) override;
    int capabilities() const override;
    AVFrame* frame() const override;
    
private:
    int decode(char* indata, int inlen);
    AVFrame* get_frame(bool native_frame);
    
    IFFmpegHardwareVideoDecoder* m_hardware_video_decoder;
    
    AVPacket m_packet;
    AVCodec* m_decoder = nullptr;
    AVCodecContext* m_decoder_context = nullptr;
    AVFrame** m_frames = nullptr;
    
    int m_frames_count;
    int m_current_frame = 0, m_next_frame = 0;
    
    char* m_ffmpeg_buffer = nullptr;
    AVFrame* m_frame = nullptr;
    pthread_mutex_t m_mutex;
};
