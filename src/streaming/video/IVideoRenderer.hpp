#include <Limelight.h>
#include "IAVFrameSyncDrawer.hpp"
#include <mutex>
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

class IVideoRenderer: public IAVFrameSyncDrawer {
public:
    virtual ~IVideoRenderer() {};
    virtual void draw() = 0;
    virtual VideoRenderStats* video_render_stats() = 0;
    
    void sync_draw(SyncDrawFunc func) {
        //std::lock_guard<std::mutex> guard(m_mutex);
        m_sync_draw_func = func;
    }
    
protected:
    SyncDrawFunc m_sync_draw_func = nullptr;
    std::mutex m_mutex;
};
