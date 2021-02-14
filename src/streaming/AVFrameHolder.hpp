#include <iostream>
#include <mutex>
#include <string>
#include <thread>

extern "C" {
    #include <libavcodec/avcodec.h>
}

#pragma once

class AVFrameHolder {
public:
    static AVFrameHolder* holder() {
        static AVFrameHolder holder;
        return &holder;
    }
    
    void push(AVFrame *frame) {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        if (m_frame) {
            av_frame_free(&m_frame);
            m_frame = nullptr;
        }
        
        if (frame == nullptr) {
            printf("frame is nullptr...\n");
            return;
        }
        
        m_frame = av_frame_alloc();
        
        if (m_frame == nullptr) {
            printf("m_frame is nullptr...\n");
            return;
        }
        
        m_frame->format = frame->format;
        m_frame->width = frame->width;
        m_frame->height = frame->height;
        m_frame->channels = frame->channels;
        m_frame->channel_layout = frame->channel_layout;
        m_frame->nb_samples = frame->nb_samples;
        
        int result = 0;
        result = av_frame_get_buffer(m_frame, 32);
        if (result != 0) {
            Logger::error("AVFrameHolder", "av_frame_get_buffer");
        }
        
        result = av_frame_copy(m_frame, frame);
        if (result != 0) {
            Logger::error("AVFrameHolder", "av_frame_copy");
        }
        
        result = av_frame_copy_props(m_frame, frame);
        if (result != 0) {
            Logger::error("AVFrameHolder", "av_frame_copy_props");
        }
    }
    
    void get(const std::function<void(AVFrame*)> fn) {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        if (m_frame) {
            fn(m_frame);
        }
    }
    
    void cleanup() {
        // TODO: plz...
    }
    
private:
    AVFrameHolder() {};
    
    std::mutex m_mutex;
    AVFrame *m_frame = nullptr;
    bool m_has_frame = false;
};

