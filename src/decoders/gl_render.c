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
in mediump vec2 tex_position;\n\
out vec4 FragColor;\n\
\
void main() {\n\
    mediump float y = texture(ymap, tex_position).r;\n\
    mediump float u = texture(umap, tex_position).r - 0.5;\n\
    mediump float v = texture(vmap, tex_position).r - 0.5;\n\
    lowp float r = y + 1.28033 * v;\n\
    lowp float g = y - 0.21482 * u - 0.38059 * v;\n\
    lowp float b = y + 2.12798 * u;\n\
    FragColor = vec4(r, g, b, 1.0);\n\
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
    
    int position = glGetAttribLocation(shader_program, "position");
    glEnableVertexAttribArray(position);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
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

void gl_render_draw(uint8_t* image[3]) {
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glUseProgram(shader_program);
    
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
