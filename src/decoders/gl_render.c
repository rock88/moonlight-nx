#include "gl_render.h"
#include "libretro.h"
#include "glsym.h"

static const char *vertex_shader_string = "\
#version 140\n\
in vec2 position;\n\
out mediump vec2 tex_position;\n\
\
void main() {\n\
    gl_Position = vec4(position, 1, 1);\n\
    tex_position = vec2((position.x + 1.0) / 2.0, (1.0 - position.y) / 2.0);\n\
}";

static const char *fragment_shader_string = "\
#version 140\n\
uniform lowp sampler2D ymap;\n\
uniform lowp sampler2D umap;\n\
uniform lowp sampler2D vmap;\n\
uniform mat3 yuvmat;\n\
uniform vec3 offset;\n\
in mediump vec2 tex_position;\n\
out vec4 FragColor;\n\
\
void main() {\n\
    vec3 YCbCr = vec3(\n\
        texture(ymap, tex_position).r,\n\
        texture(umap, tex_position).r - 0.0,\n\
        texture(vmap, tex_position).r - 0.0\n\
    );\n\
    YCbCr -= offset;\n\
    FragColor = vec4(clamp(yuvmat * YCbCr, 0.0, 1.0), 1.0);\n\
}";

static const float vertices[] = {
    -1.f, -1.f,
    1.f, -1.f,
    -1.f, 1.f,
    1.f, 1.f
};

static const char* texture_mappings[] = { "ymap", "umap", "vmap" };
static GLuint texture_id[3], texture_uniform[3];
static GLuint shader_program;
static GLuint vbo, vao;
static int gl_render_width = 0, gl_render_height = 0;
static int yuvmat_location, offset_location;

void gl_render_init() {
    shader_program = glCreateProgram();
    GLuint vert = glCreateShader(GL_VERTEX_SHADER);
    GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
    
    glShaderSource(vert, 1, &vertex_shader_string, 0);
    glCompileShader(vert);
    
    glShaderSource(frag, 1, &fragment_shader_string, 0);
    glCompileShader(frag);
    
    glAttachShader(shader_program, vert);
    glAttachShader(shader_program, frag);
    glLinkProgram(shader_program);
    glDeleteShader(vert);
    glDeleteShader(frag);
    
    for (int i = 0; i < 3; i++) {
        texture_uniform[i] = glGetUniformLocation(shader_program, texture_mappings[i]);
    }
    
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    glUseProgram(shader_program);
    
    int position_location = glGetAttribLocation(shader_program, "position");
    glEnableVertexAttribArray(position_location);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    
    yuvmat_location = glGetUniformLocation(shader_program, "yuvmat");
    offset_location = glGetUniformLocation(shader_program, "offset");
}

void gl_render_setup(int width, int height) {
    if (gl_render_width != width || gl_render_height != height) {
        glDeleteTextures(3, texture_id);
    } else {
        return;
    }
    
    gl_render_width = width;
    gl_render_height = height;
    
    glGenTextures(3, texture_id);
    
    for (int i = 0; i < 3; i++) {
        glBindTexture(GL_TEXTURE_2D, texture_id[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, i > 0 ? width / 2 : width, i > 0 ? height / 2 : height, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
    }
}

const float* gl_color_offset(bool color_full) {
    static const float limitedOffsets[] = { 16.0f / 255.0f, 128.0f / 255.0f, 128.0f / 255.0f };
    static const float fullOffsets[] = { 0.0f, 128.0f / 255.0f, 128.0f / 255.0f };
    return color_full ? fullOffsets : limitedOffsets;
}

const float* gl_color_matrix(enum AVColorSpace color_space, bool color_full) {
    static const float bt601Lim[] = {
        1.1644f, 1.1644f, 1.1644f,
        0.0f, -0.3917f, 2.0172f,
        1.5960f, -0.8129f, 0.0f
    };
    static const float bt601Full[] = {
        1.0f, 1.0f, 1.0f,
        0.0f, -0.3441f, 1.7720f,
        1.4020f, -0.7141f, 0.0f
    };
    static const float bt709Lim[] = {
        1.1644f, 1.1644f, 1.1644f,
        0.0f, -0.2132f, 2.1124f,
        1.7927f, -0.5329f, 0.0f
    };
    static const float bt709Full[] = {
        1.0f, 1.0f, 1.0f,
        0.0f, -0.1873f, 1.8556f,
        1.5748f, -0.4681f, 0.0f
    };
    static const float bt2020Lim[] = {
        1.1644f, 1.1644f, 1.1644f,
        0.0f, -0.1874f, 2.1418f,
        1.6781f, -0.6505f, 0.0f
    };
    static const float bt2020Full[] = {
        1.0f, 1.0f, 1.0f,
        0.0f, -0.1646f, 1.8814f,
        1.4746f, -0.5714f, 0.0f
    };
    
    switch (color_space) {
        case AVCOL_SPC_SMPTE170M:
        case AVCOL_SPC_BT470BG:
            return color_full ? bt601Full : bt601Lim;
        case AVCOL_SPC_BT709:
            return color_full ? bt709Full : bt709Lim;
        case AVCOL_SPC_BT2020_NCL:
        case AVCOL_SPC_BT2020_CL:
            return color_full ? bt2020Full : bt2020Lim;
        default:
            return bt601Lim;
    };
}

void gl_render_draw(uint8_t* image[3], enum AVColorSpace color_space, enum AVColorRange color_range) {
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glUseProgram(shader_program);
    
    glUniform3fv(offset_location, 1, gl_color_offset(color_range == AVCOL_RANGE_JPEG));
    glUniformMatrix3fv(yuvmat_location, 1, GL_FALSE, gl_color_matrix(color_space, color_range == AVCOL_RANGE_JPEG));
    
    for (int i = 0; i < 3; i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, texture_id[i]);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, i > 0 ? gl_render_width / 2 : gl_render_width, i > 0 ? gl_render_height / 2 : gl_render_height, GL_RED, GL_UNSIGNED_BYTE, image[i]);
        glUniform1i(texture_uniform[i], i);
        glActiveTexture(GL_TEXTURE0);
    }
    
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
