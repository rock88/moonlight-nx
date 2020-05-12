#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <curl/curl.h>
#include <openssl/ssl.h>

#include "glsym/glsym.h"
#include "libretro.h"
#include "Application.hpp"
#include "InputController.hpp"
#include "Settings.hpp"

extern "C" {
    #include "client.h"
}

struct retro_hw_render_callback hw_render;

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
retro_audio_sample_batch_t audio_batch_cb;
static retro_environment_t environ_cb;
static retro_input_poll_t input_poll_cb;
retro_input_state_t input_state_cb;

static Application* app;
static bool moonlight_is_initialized = false;

void moonlight_init(int width, int height) {
    if (moonlight_is_initialized) {
        return;
    }
    
    moonlight_is_initialized = true;
    
    nanogui::init();
    app = new Application(Size(width, height), Size(width, height));
    
    nanogui::setup(1.0 / 15.0);
}

void retro_init(void) {
    #ifdef __LAKKA_SWITCH__
    mkdirtree("/storage/system/moonlight");
    Settings::settings()->set_working_dir("/storage/system/moonlight");
    #endif
    
    OpenSSL_add_all_algorithms();
    curl_global_init(CURL_GLOBAL_ALL);
}

void retro_deinit(void) {
    
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
    info->library_version  = "v" MOONLIGHT_LIBRETRO_VERSION;
    info->need_fullpath    = false;
    info->valid_extensions = NULL; // Anything is fine, we don't care.
}

void retro_get_system_av_info(struct retro_system_av_info *info) {
    info->timing = (struct retro_system_timing) {
        .fps = 60.0,
        .sample_rate = 48000,
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
    
#ifndef __LAKKA_SWITCH__
    const char *dir = NULL;
    cb(RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY, &dir);
    if (dir != NULL) {
        Settings::settings()->set_working_dir(dir);
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
    // Nothing here...
}

void retro_run(void) {
    moonlight_init(width, height);
    
    input_poll_cb();
    InputController::controller()->handle_input(width, height);
    
    glBindFramebuffer(RARCH_GL_FRAMEBUFFER, hw_render.get_current_framebuffer());
    
    glViewport(0, 0, width, height);
    nanogui::draw();
    
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
    hw_render.version_minor = 2;
#else
    hw_render.context_type = RETRO_HW_CONTEXT_OPENGL;
#endif
    hw_render.context_reset = context_reset;
    hw_render.context_destroy = context_destroy;
    hw_render.depth = true;
    hw_render.stencil = true;
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
