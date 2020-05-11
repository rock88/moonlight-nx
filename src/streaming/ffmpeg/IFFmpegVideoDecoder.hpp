#include <Limelight.h>
#pragma once

extern "C" {
    #include <libavcodec/avcodec.h>
}

class IFFmpegVideoDecoder {
public:
    virtual ~IFFmpegVideoDecoder() {};
    virtual int setup(int video_format, int width, int height, int redraw_rate, void* context, int dr_flags) = 0;
    virtual void start() {};
    virtual void stop() {};
    virtual void cleanup() = 0;
    virtual int submit_decode_unit(PDECODE_UNIT decode_unit) = 0;
    virtual int capabilities() const = 0;
    virtual AVFrame* frame() const = 0;
};
