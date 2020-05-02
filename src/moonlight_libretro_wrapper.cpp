#include "moonlight_libretro_wrapper.h"
#include "Application.hpp"
#include "Server.hpp"
#include "gl_render.h"
#include <openssl/ssl.h>
#include <curl/curl.h>

static bool moonlight_is_initialized = false;

static Application* app;

void moonlight_libretro_wrapper_init(int width, int height) {
    if (moonlight_is_initialized) {
        return;
    }
    
    moonlight_is_initialized = true;
    
    OpenSSL_add_all_algorithms();
    curl_global_init(CURL_GLOBAL_ALL);
    
    gl_render_init();
    
    nanogui::init();
    app = new Application(Size(width, height), Size(width, height));
    
    nanogui::setup(1.0 / 60.0);
}

void moonlight_libretro_wrapper_set_working_dir(const char* dir) {
    Server::server()->set_working_dir(dir);
}

void moonlight_libretro_wrapper_handle_mouse_move(double x, double y) {
    nanogui::cursor_pos_callback_event(x, y);
}

void moonlight_libretro_wrapper_handle_mouse_button(int button, int action, int modifiers) {
    nanogui::mouse_button_callback_event(button, action, modifiers);
}

void moonlight_libretro_wrapper_draw() {
    nanogui::draw();
}
