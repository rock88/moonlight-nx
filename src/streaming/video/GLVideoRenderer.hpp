#include "IVideoRenderer.hpp"
#include <glad/glad.h>
#include <vector>
#pragma once

class GLVideoRenderer: public IVideoRenderer {
public:
    GLVideoRenderer();
    ~GLVideoRenderer();
    
    void draw() override;
    
    VideoRenderStats* video_render_stats() override;
    
private:
    void initialize();
    
    bool m_is_initialized = false;
    GLuint m_texture_id[3] = {0, 0, 0}, m_texture_uniform[3];
    GLuint m_shader_program;
    GLuint m_vbo, m_vao;
    int m_width = 0, m_height = 0;
    int m_yuvmat_location, m_offset_location;
    std::vector<float> m_gl_color_offset;
    std::vector<float> m_gl_color_matrix;
    VideoRenderStats m_video_render_stats = {};
};
