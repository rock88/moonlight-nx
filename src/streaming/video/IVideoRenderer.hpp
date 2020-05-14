#include <Limelight.h>
#pragma once

extern "C" {
    #include <libavcodec/avcodec.h>
}

struct VideoRenderStats {
    uint32_t rendered_frames;
    uint64_t total_render_time;
    float rendered_fps;
    double measurement_start_timestamp;
};

class IVideoRenderer {
public:
    virtual ~IVideoRenderer() {};
    virtual void draw(int width, int height, AVFrame* frame) = 0;
    virtual VideoRenderStats* video_render_stats() = 0;
};
