#include <stdio.h>
#include <string>
#include <vector>
#pragma once

#define MOONLIGHT_LIBRETRO_VERSION "1.0.0"

class Settings {
public:
    static Settings* settings() {
        static Settings settings;
        return &settings;
    }
    
    std::string working_dir() const {
        return m_working_dir;
    }
    
    void set_working_dir(std::string working_dir) {
        m_working_dir = std::string(working_dir);
        load();
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
    
    void load();
    void save();

private:
    Settings() {};
    
    std::string m_working_dir;
    std::vector<std::string> m_hosts;
    int m_resolution = 720;
    int m_fps = 30;
    int m_bitrate = 500;
    bool m_swap_ab_xy = true;
};
