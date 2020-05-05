#include "Settings.hpp"
#include "json.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>

#define TRY_JSON(x) try { \
    x; } catch (const std::exception &e) {}

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
    
    TRY_JSON(m_hosts = json["hosts"].get<std::vector<std::string>>());
    TRY_JSON(m_resolution = json["settings"]["resolution"].get<int>());
    TRY_JSON(m_fps = json["settings"]["fps"].get<int>());
    TRY_JSON(m_bitrate = json["settings"]["bitrate"].get<int>());
    TRY_JSON(m_swap_ab_xy = json["settings"]["swap_ab_xy"].get<bool>());
}

void Settings::save() {
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
}
