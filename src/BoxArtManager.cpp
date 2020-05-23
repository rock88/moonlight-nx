#include "BoxArtManager.hpp"
#include "Settings.hpp"
#include "nanovg.h"
#include <algorithm>
#include <mutex>

#define BLANK_IMAGE_SIZE 6129

std::mutex m_mutex;

bool BoxArtManager::has_boxart(int app_id) {
    if (m_has_boxart.count(app_id)) {
        return m_has_boxart[app_id];
    }
    
    std::string path = Settings::settings()->boxart_dir() + "/" + std::to_string(app_id) + ".png";
    Data data = Data::read_from_file(path);
    m_has_boxart[app_id] = !data.is_empty();
    
    if (data.size() == BLANK_IMAGE_SIZE) {
        m_blanks.push_back(app_id);
    }
    return m_has_boxart[app_id];
}

bool BoxArtManager::is_blank(int app_id) {
    return std::find(m_blanks.begin(), m_blanks.end(), app_id) != m_blanks.end();
}

void BoxArtManager::set_data(Data data, int app_id) {
    std::lock_guard<std::mutex> guard(m_mutex);
    
    std::string path = Settings::settings()->boxart_dir() + "/" + std::to_string(app_id) + ".png";
    data.write_to_file(path);
    m_has_boxart[app_id] = true;
    
    if (data.size() == BLANK_IMAGE_SIZE) {
        m_blanks.push_back(app_id);
    }
}

void BoxArtManager::make_texture_from_boxart(NVGcontext *ctx, int app_id) {
    std::lock_guard<std::mutex> guard(m_mutex);
    
    std::string path = Settings::settings()->boxart_dir() + "/" + std::to_string(app_id) + ".png";
    Data data = Data::read_from_file(path);
    
    int handle = nvgCreateImageMem(ctx, 0, data.bytes(), (int)data.size());
    m_texture_handle[app_id] = handle;
}

int BoxArtManager::texture_id(int app_id) {
    if (m_texture_handle.count(app_id)) {
        return m_texture_handle[app_id];
    }
    return -1;
}
