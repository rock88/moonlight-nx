#include "Settings.hpp"
#include "json.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>

#define LOG printf("%s:%i\n", __FUNCTION__, __LINE__);

#define JSON_GET_VALUE(to, json, check, type) \
    if (json.check()) { \
        to = json.get<type>(); \
    }

void Settings::add_host(const std::string address) {
    LOG
    if (std::find(m_hosts.begin(), m_hosts.end(), address) == m_hosts.end()) {
        LOG
        m_hosts.push_back(address);
    }
    LOG
    save();
    LOG
}

void Settings::load() {
    LOG
    std::ifstream stream(m_working_dir + "/settings.json");
    LOG
    if (!stream) {
        LOG
        return;
    }
    
    try {
        LOG
        nlohmann::json json;
        LOG
        stream >> json;
        LOG
        JSON_GET_VALUE(m_hosts, json["hosts"], is_array, std::vector<std::string>);
        JSON_GET_VALUE(m_resolution, json["settings"]["resolution"], is_number_integer, int);
        JSON_GET_VALUE(m_fps, json["settings"]["fps"], is_number_integer, int);
        JSON_GET_VALUE(m_video_codec, json["settings"]["video_codec"], is_number_integer, VideoCodec);
        JSON_GET_VALUE(m_bitrate, json["settings"]["bitrate"], is_number_integer, int);
        JSON_GET_VALUE(m_swap_ab_xy, json["settings"]["swap_ab_xy"], is_number_integer, bool);
        LOG
    } catch (const std::exception &e) {
        printf("Load settings error: %s\n", e.what());
    }
}

void Settings::save() {
    LOG
    try {
        nlohmann::json json;
        
        json["hosts"] = m_hosts;
        json["settings"] = {
            {"resolution", m_resolution},
            {"fps", m_fps},
            {"video_codec", m_video_codec},
            {"bitrate", m_bitrate},
            {"swap_ab_xy", m_swap_ab_xy}
        };
        LOG
        std::ofstream stream(m_working_dir + "/settings.json");
        LOG
        stream << std::setw(4) << json << std::endl;
        LOG
    } catch (const std::exception &e) {
        printf("Save settings error: %s\n", e.what());
    }
}
