#include <stdio.h>
#include <string>
#include <map>
#include <vector>
#include "Data.hpp"
#pragma once

struct NVGcontext;

class BoxArtManager {
public:
    static BoxArtManager* manager() {
        static BoxArtManager manager;
        return &manager;
    }
    
    bool has_boxart(int app_id);
    bool is_blank(int app_id);
    
    void set_data(Data data, int app_id);
    void make_texture_from_boxart(NVGcontext *ctx, int app_id);
    int texture_id(int app_id);
    
private:
    BoxArtManager() {};
    
    std::map<int, bool> m_has_boxart;
    std::map<int, int> m_texture_handle;
    std::vector<int> m_blanks;
};
