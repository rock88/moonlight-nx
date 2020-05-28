#include "Settings.hpp"
#include "Log.h"
#include <jansson.h>
#include <algorithm>
#include <string.h>
#include <iomanip>
#include <limits.h>
#include <sys/stat.h>

static int mkdirtree(const char* directory) {
    char buffer[PATH_MAX];
    char* p = buffer;
    
    // The passed in string could be a string literal
    // so we must copy it first
    strncpy(p, directory, PATH_MAX - 1);
    buffer[PATH_MAX - 1] = '\0';
    
    while (*p != 0) {
        // Find the end of the path element
        do {
            p++;
        } while (*p != 0 && *p != '/');
        
        char oldChar = *p;
        *p = 0;
        
        // Create the directory if it doesn't exist already
        if (mkdir(buffer, 0775) == -1 && errno != EEXIST) {
            return -1;
        }
        
        *p = oldChar;
    }
    return 0;
}

void Settings::set_working_dir(std::string working_dir) {
    m_working_dir = working_dir;
    m_key_dir = working_dir + "/key";
    m_boxart_dir = working_dir + "/boxart";
    
    mkdirtree(m_working_dir.c_str());
    mkdirtree(m_key_dir.c_str());
    mkdirtree(m_boxart_dir.c_str());
    
    load();
}

void Settings::add_host(const std::string address) {
    if (std::find(m_hosts.begin(), m_hosts.end(), address) == m_hosts.end()) {
        m_hosts.push_back(address);
        save();
    }
}

void Settings::load() {
    json_t* root = json_load_file((m_working_dir + "/settings.json").c_str(), 0, NULL);
    
    if (root && json_typeof(root) == JSON_OBJECT) {
        if (json_t* hosts = json_object_get(root, "hosts")) {
            size_t size = json_array_size(hosts);
            for (size_t i = 0; i < size; i++) {
                if (json_t* host = json_array_get(hosts, i)) {
                    if (json_typeof(host) == JSON_STRING) {
                        m_hosts.push_back(json_string_value(host));
                    }
                }
            }
        }
        
        if (json_t* settings = json_object_get(root, "settings")) {
            if (json_t* resolution = json_object_get(settings, "resolution")) {
                if (json_typeof(resolution) == JSON_INTEGER) {
                    m_resolution = (int)json_integer_value(resolution);
                }
            }
            
            if (json_t* fps = json_object_get(settings, "fps")) {
                if (json_typeof(fps) == JSON_INTEGER) {
                    m_fps = (int)json_integer_value(fps);
                }
            }
            
            if (json_t* video_codec = json_object_get(settings, "video_codec")) {
                if (json_typeof(video_codec) == JSON_INTEGER) {
                    m_video_codec = (VideoCodec)json_integer_value(video_codec);
                }
            }
            
            if (json_t* bitrate = json_object_get(settings, "bitrate")) {
                if (json_typeof(bitrate) == JSON_INTEGER) {
                    m_bitrate = (int)json_integer_value(bitrate);
                }
            }
            
            if (json_t* swap_ab_xy = json_object_get(settings, "swap_ab_xy")) {
                m_swap_ab_xy = json_typeof(swap_ab_xy) == JSON_TRUE;
            }
            
            if (json_t* decoder_threads = json_object_get(settings, "decoder_threads")) {
                if (json_typeof(decoder_threads) == JSON_INTEGER) {
                    m_decoder_threads = (int)json_integer_value(decoder_threads);
                }
            }
            
            if (json_t* sops = json_object_get(settings, "sops")) {
                m_sops = json_typeof(sops) == JSON_TRUE;
            }
            
            if (json_t* play_audio = json_object_get(settings, "play_audio")) {
                m_play_audio = json_typeof(play_audio) == JSON_TRUE;
            }
        }
        
        json_decref(root);
    }
}

void Settings::save() {
    json_t* root = json_object();
    
    if (root) {
        if (json_t* hosts = json_array()) {
            for (auto host: m_hosts) {
                json_array_append(hosts, json_string(host.c_str()));
            }
            json_object_set(root, "hosts", hosts);
        }
        
        if (json_t* settings = json_object()) {
            json_object_set(settings, "resolution", json_integer(m_resolution));
            json_object_set(settings, "fps", json_integer(m_fps));
            json_object_set(settings, "video_codec", json_integer(m_video_codec));
            json_object_set(settings, "bitrate", json_integer(m_bitrate));
            json_object_set(settings, "decoder_threads", json_integer(m_decoder_threads));
            json_object_set(settings, "swap_ab_xy", m_swap_ab_xy ? json_true() : json_false());
            json_object_set(settings, "sops", m_sops ? json_true() : json_false());
            json_object_set(settings, "play_audio", m_play_audio ? json_true() : json_false());
            json_object_set(root, "settings", settings);
        }
        
        json_dump_file(root, (m_working_dir + "/settings.json").c_str(), JSON_INDENT(4));
        json_decref(root);
    }
}
