#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "glsym/glsym.h"
#include "libretro.h"
#include "moonlight_libretro_wrapper.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
static struct retro_hw_render_callback hw_render;

#if defined(HAVE_PSGL)
#define RARCH_GL_FRAMEBUFFER GL_FRAMEBUFFER_OES
#define RARCH_GL_FRAMEBUFFER_COMPLETE GL_FRAMEBUFFER_COMPLETE_OES
#define RARCH_GL_COLOR_ATTACHMENT0 GL_COLOR_ATTACHMENT0_EXT
#elif defined(OSX_PPC)
#define RARCH_GL_FRAMEBUFFER GL_FRAMEBUFFER_EXT
#define RARCH_GL_FRAMEBUFFER_COMPLETE GL_FRAMEBUFFER_COMPLETE_EXT
#define RARCH_GL_COLOR_ATTACHMENT0 GL_COLOR_ATTACHMENT0_EXT
#else
#define RARCH_GL_FRAMEBUFFER GL_FRAMEBUFFER
#define RARCH_GL_FRAMEBUFFER_COMPLETE GL_FRAMEBUFFER_COMPLETE
#define RARCH_GL_COLOR_ATTACHMENT0 GL_COLOR_ATTACHMENT0
#endif

#define BASE_WIDTH 1280
#define BASE_HEIGHT 720
#define MAX_WIDTH 1280
#define MAX_HEIGHT 720

static unsigned width  = BASE_WIDTH;
static unsigned height = BASE_HEIGHT;

static retro_video_refresh_t video_cb;
static retro_audio_sample_t audio_cb;
static retro_audio_sample_batch_t audio_batch_cb;
static retro_environment_t environ_cb;
static retro_input_poll_t input_poll_cb;
static retro_input_state_t input_state_cb;
static retro_log_printf_t log_cb;

void retro_init(void) {
    //moonlight_libretro_wrapper_init();
}

void retro_deinit(void){
    
}

unsigned retro_api_version(void) {
    return RETRO_API_VERSION;
}

void retro_set_controller_port_device(unsigned port, unsigned device) {
    (void)port;
    (void)device;
}

void retro_get_system_info(struct retro_system_info *info) {
    memset(info, 0, sizeof(*info));
    info->library_name     = "Moonlight";
    info->library_version  = "v1";
    info->need_fullpath    = true;
    info->valid_extensions = NULL; // Anything is fine, we don't care.
}

void retro_get_system_av_info(struct retro_system_av_info *info) {
    info->timing = (struct retro_system_timing) {
        .fps = 60.0,
        .sample_rate = 0.0,
    };
    
    info->geometry = (struct retro_game_geometry) {
        .base_width   = BASE_WIDTH,
        .base_height  = BASE_HEIGHT,
        .max_width    = MAX_WIDTH,
        .max_height   = MAX_HEIGHT,
        .aspect_ratio = 16.0 / 9.0,
    };
}

void retro_set_environment(retro_environment_t cb) {
    environ_cb = cb;
    
    bool no_game = true;
    cb(RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME, &no_game);
    
#ifdef __LAKKA_SWITCH__
    moonlight_libretro_wrapper_set_working_dir("/storage/cores");
#else
    const char *dir = NULL;
    cb(RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY, &dir);
    if (dir != NULL) {
        moonlight_libretro_wrapper_set_working_dir(dir);
    }
#endif
}

void retro_set_audio_sample(retro_audio_sample_t cb) {
    audio_cb = cb;
}

void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb) {
    audio_batch_cb = cb;
}

void retro_set_input_poll(retro_input_poll_t cb) {
    input_poll_cb = cb;
}

void retro_set_input_state(retro_input_state_t cb) {
    input_state_cb = cb;
}

void retro_set_video_refresh(retro_video_refresh_t cb) {
    video_cb = cb;
}

static void update_variables(void) {
    bool updated = false;
    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &updated) && updated) {
        
    }
}

void retro_run(void) {
    moonlight_libretro_wrapper_init(width, height);
    
    // Handle inputs
    input_poll_cb();
    
    double mouse_x = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_X);
    double mouse_y = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_Y);
    double pointer_x = 0;
    double pointer_y = 0;
    
    // TODO: Pointers in MacBook currently work incorrectly...
    if (input_state_cb(0, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_PRESSED)) {
        int p_x = input_state_cb(0, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_X);
        int p_y = input_state_cb(0, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_Y);
        pointer_x = (p_x + 0x7fff) * width / 0xffff;
        pointer_y = (p_y + 0x7fff) * height / 0xffff;
    }
    
    if (mouse_x != 0 && mouse_y != 0) {
        moonlight_libretro_wrapper_handle_mouse_move(mouse_x, mouse_y);
    }
    
    if (input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_LEFT)) {
        if (pointer_x != 0 && pointer_y != 0) {
            moonlight_libretro_wrapper_handle_mouse_move(pointer_x, pointer_y);
        }
        
        moonlight_libretro_wrapper_handle_mouse_button(0, 1, 0);
    } else if (!input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_LEFT)) {
        moonlight_libretro_wrapper_handle_mouse_button(0, 0, 0);
    }
    
    // Draw
    glBindFramebuffer(RARCH_GL_FRAMEBUFFER, hw_render.get_current_framebuffer());
    
    glViewport(0, 0, width, height);
    moonlight_libretro_wrapper_draw();
    
    video_cb(RETRO_HW_FRAME_BUFFER_VALID, width, height, 0);
}

static void context_reset(void) {
    fprintf(stderr, "Context reset!\n");
    rglgen_resolve_symbols(hw_render.get_proc_address);
}

static void context_destroy(void) {
    fprintf(stderr, "Context destroy!\n");
}

static bool retro_init_hw_context(void) {
#if 1
    hw_render.context_type = RETRO_HW_CONTEXT_OPENGL_CORE;
    hw_render.version_major = 3;
    hw_render.version_minor = 1;
#else
    hw_render.context_type = RETRO_HW_CONTEXT_OPENGL;
#endif
    hw_render.context_reset = context_reset;
    hw_render.context_destroy = context_destroy;
    hw_render.bottom_left_origin = true;
    
    if (!environ_cb(RETRO_ENVIRONMENT_SET_HW_RENDER, &hw_render))
        return false;
    
    return true;
}

bool retro_load_game(const struct retro_game_info *info) {
    update_variables();
    
    enum retro_pixel_format fmt = RETRO_PIXEL_FORMAT_XRGB8888;
    if (!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &fmt)) {
        fprintf(stderr, "XRGB8888 is not supported.\n");
        return false;
    }
    
    if (!retro_init_hw_context()) {
        fprintf(stderr, "HW Context could not be initialized, exiting...\n");
        return false;
    }
    
    fprintf(stderr, "Loaded game!\n");
    (void)info;
    return true;
}

void retro_unload_game(void) {
    
}

unsigned retro_get_region(void) {
    return RETRO_REGION_NTSC;
}

bool retro_load_game_special(unsigned type, const struct retro_game_info *info, size_t num) {
    (void)type;
    (void)info;
    (void)num;
    return false;
}

size_t retro_serialize_size(void) {
    return 0;
}

bool retro_serialize(void *data, size_t size) {
    (void)data;
    (void)size;
    return false;
}

bool retro_unserialize(const void *data, size_t size) {
    (void)data;
    (void)size;
    return false;
}

void *retro_get_memory_data(unsigned id) {
    (void)id;
    return NULL;
}

size_t retro_get_memory_size(unsigned id) {
    (void)id;
    return 0;
}

void retro_reset(void){
    
}

void retro_cheat_reset(void){
    
}

void retro_cheat_set(unsigned index, bool enabled, const char *code) {
    (void)index;
    (void)enabled;
    (void)code;
}
