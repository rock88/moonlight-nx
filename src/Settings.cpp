#include "Settings.hpp"
#include "json.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>

#define JSON_GET_VALUE(to, json, check, type) \
    if (json.check()) { \
        to = json.get<type>(); \
    }

void Settings::add_host(const std::string address) {
    if (std::find(m_hosts.begin(), m_hosts.end(), address) == m_hosts.end()) {
        m_hosts.push_back(address);
    }
    save();
}

void Settings::load() {
    std::ifstream stream(m_working_dir + "/settings.json");
    
    if (!stream) {
        return;
    }
    
    nlohmann::json json;
    stream >> json;
    
    JSON_GET_VALUE(m_hosts, json["hosts"], is_array, std::vector<std::string>);
    JSON_GET_VALUE(m_resolution, json["settings"]["resolution"], is_number_integer, int);
    JSON_GET_VALUE(m_fps, json["settings"]["fps"], is_number_integer, int);
    JSON_GET_VALUE(m_bitrate, json["settings"]["bitrate"], is_number_integer, int);
    JSON_GET_VALUE(m_swap_ab_xy, json["settings"]["swap_ab_xy"], is_number_integer, bool);
}

void Settings::save() {
    try {
        nlohmann::json json;
        
        json["hosts"] = m_hosts;
        json["settings"] = {
            {"resolution", m_resolution},
            {"fps", m_fps},
            {"bitrate", m_bitrate},
            {"swap_ab_xy", m_swap_ab_xy}
        };
        
        std::ofstream stream(m_working_dir + "/settings.json");
        stream << std::setw(4) << json << std::endl;
    } catch (const std::exception &e) {
        printf("Save settings error: %s\n", e.what());
    }
}
