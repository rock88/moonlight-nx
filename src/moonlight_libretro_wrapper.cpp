#include "moonlight_libretro_wrapper.h"
#include "libretro.h"
#include "gl_render.h"
#include "Application.hpp"
#include "GameStreamClient.hpp"
#include "Limelight.h"
#include <curl/curl.h>
#include <openssl/ssl.h>

static bool moonlight_is_initialized = false;

static Application* app;
static std::string working_dir;

int16_t keyboard_state[RETROK_LAST];
struct GamePadState game_pad_state;

extern retro_input_state_t input_state_cb;

void moonlight_libretro_wrapper_preinit() {
    OpenSSL_add_all_algorithms();
    curl_global_init(CURL_GLOBAL_ALL);
}

void moonlight_libretro_wrapper_init(int width, int height) {
    if (moonlight_is_initialized) {
        return;
    }
    
    moonlight_is_initialized = true;
    
    GameStreamClient::client()->set_working_dir(working_dir);
    
    gl_render_init();
    
    nanogui::init();
    app = new Application(Size(width, height), Size(width, height));
    
    //nanogui::setup(1.0 / 60.0 * 1000); no needs anymore?
}

void moonlight_libretro_wrapper_set_working_dir(const char* dir) {
    LOG("Set working dir: %s\n", dir);
    std::string str(dir, strlen(dir));
    working_dir = str;
}

void moonlight_libretro_wrapper_handle_mouse_move(double x, double y) {
    nanogui::cursor_pos_callback_event(x, y);
}

void moonlight_libretro_wrapper_handle_mouse_button(int button, int action, int modifiers) {
    nanogui::mouse_button_callback_event(button, action, modifiers);
}

static void set_game_pad_state(short flag, int16_t state) {
    if (state) {
        game_pad_state.buttonFlags |= flag;
    } else {
        game_pad_state.buttonFlags &= ~flag;
    }
}

void moonlight_libretro_wrapper_handle_game_pad() {
    game_pad_state.leftTrigger = 0xFFFF * input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L2);
    game_pad_state.rightTrigger = 0xFFFF * input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R2);
    
    game_pad_state.leftStickX = input_state_cb(0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_X);
    game_pad_state.leftStickY = 0xFFFF - input_state_cb(0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_Y);
    game_pad_state.rightStickX = input_state_cb(0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_X);
    game_pad_state.rightStickY = 0xFFFF - input_state_cb(0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_Y);

    #define GAME_PAD_STATE(KEY) \
        input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, KEY)
    
    set_game_pad_state(UP_FLAG, GAME_PAD_STATE(RETRO_DEVICE_ID_JOYPAD_UP));
    set_game_pad_state(DOWN_FLAG, GAME_PAD_STATE(RETRO_DEVICE_ID_JOYPAD_DOWN));
    set_game_pad_state(LEFT_FLAG, GAME_PAD_STATE(RETRO_DEVICE_ID_JOYPAD_LEFT));
    set_game_pad_state(RIGHT_FLAG, GAME_PAD_STATE(RETRO_DEVICE_ID_JOYPAD_RIGHT));
    
    set_game_pad_state(BACK_FLAG, GAME_PAD_STATE(RETRO_DEVICE_ID_JOYPAD_SELECT));
    set_game_pad_state(PLAY_FLAG, GAME_PAD_STATE(RETRO_DEVICE_ID_JOYPAD_START));
    
    set_game_pad_state(LB_FLAG, GAME_PAD_STATE(RETRO_DEVICE_ID_JOYPAD_L));
    set_game_pad_state(RB_FLAG, GAME_PAD_STATE(RETRO_DEVICE_ID_JOYPAD_R));
    
    set_game_pad_state(LS_CLK_FLAG, GAME_PAD_STATE(RETRO_DEVICE_ID_JOYPAD_L3));
    set_game_pad_state(RS_CLK_FLAG, GAME_PAD_STATE(RETRO_DEVICE_ID_JOYPAD_R3));
    
    #if 0
    set_game_pad_state(A_FLAG, GAME_PAD_STATE(RETRO_DEVICE_ID_JOYPAD_A));
    set_game_pad_state(B_FLAG, GAME_PAD_STATE(RETRO_DEVICE_ID_JOYPAD_B));
    set_game_pad_state(X_FLAG, GAME_PAD_STATE(RETRO_DEVICE_ID_JOYPAD_X));
    set_game_pad_state(Y_FLAG, GAME_PAD_STATE(RETRO_DEVICE_ID_JOYPAD_Y));
    #else
    // Swap A/B & X/Y
    set_game_pad_state(A_FLAG, GAME_PAD_STATE(RETRO_DEVICE_ID_JOYPAD_B));
    set_game_pad_state(B_FLAG, GAME_PAD_STATE(RETRO_DEVICE_ID_JOYPAD_A));
    set_game_pad_state(X_FLAG, GAME_PAD_STATE(RETRO_DEVICE_ID_JOYPAD_Y));
    set_game_pad_state(Y_FLAG, GAME_PAD_STATE(RETRO_DEVICE_ID_JOYPAD_X));
    #endif
}

void moonlight_libretro_wrapper_draw() {
    nanogui::draw();
}
