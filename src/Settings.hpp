#include <stdio.h>
#include <string>
#include <vector>
#pragma once

#define MOONLIGHT_LIBRETRO_VERSION "1.0.7"

enum VideoCodec: int {
    H264,
    H265
};

class Settings {
public:
    static Settings* settings() {
        static Settings settings;
        return &settings;
    }
    
    void set_working_dir(std::string working_dir);
    
    std::string key_dir() const {
        return m_key_dir;
    }
    
    std::string boxart_dir() const {
        return m_boxart_dir;
    }
    
    std::vector<std::string> hosts() const {
        return m_hosts;
    }
    
    void add_host(const std::string address);
    
    int resolution() const {
        return m_resolution;
    }
    
    void set_resolution(int resolution) {
        m_resolution = resolution;
    }
    
    int fps() const {
        return m_fps;
    }
    
    void set_fps(int fps) {
        m_fps = fps;
    }
    
    VideoCodec video_codec() const {
        return m_video_codec;
    }
    
    void set_video_codec(VideoCodec video_codec) {
        m_video_codec = video_codec;
    }
    
    int bitrate() const {
        return m_bitrate;
    }
    
    void set_bitrate(int bitrate) {
        m_bitrate = bitrate;
    }
    
    bool swap_ab_xy() const {
        return m_swap_ab_xy;
    }
    
    void set_swap_ab_xy(bool swap_ab_xy) {
        m_swap_ab_xy = swap_ab_xy;
    }
    
    void set_decoder_threads(int decoder_threads) {
        m_decoder_threads = decoder_threads;
    }
    
    int decoder_threads() const {
        return m_decoder_threads;
    }
    
    void load();
    void save();

private:
    Settings() {};
    
    std::string m_working_dir;
    std::string m_key_dir;
    std::string m_boxart_dir;
    
    std::vector<std::string> m_hosts;
    int m_resolution = 720;
    int m_fps = 60;
    VideoCodec m_video_codec = H264;
    int m_bitrate = 10000;
    bool m_swap_ab_xy = true;
    int m_decoder_threads = 4;
};
