#include <stdio.h>
#include <stdlib.h>
#include "Application.hpp"
#include "Settings.hpp"
#include "Limelight.h"
#include "libretro.h"
#include "InputController.hpp"

#ifdef __SWITCH__
#include <glad/glad.h>
#include <switch.h>
#else
#include "glsym/glsym.h"
#endif

#include <GLFW/glfw3.h>

#ifdef __SWITCH__
retro_input_state_t input_state_cb;
#else
extern retro_input_state_t input_state_cb;
#endif

static int mouse_x = 0, mouse_y = 0;
static int mouse_l = 0, mouse_r = 0;

static int glfw_keyboard_state[GLFW_KEY_LAST];
static GLFWgamepadstate glfw_gamepad_state;
int moonlight_exit = 0;

#define GLFW_KEY_TO_RETRO(RETRO, KEY) \
    if (id == RETRO) return glfw_keyboard_state[KEY];

#define GLFW_GAMEPAD_TO_RETRO(RETRO, KEY) \
    if (id == RETRO) return glfw_gamepad_state.buttons[KEY];

static int16_t glfw_input_state_cb(unsigned port, unsigned device, unsigned index, unsigned id) {
    if (device == RETRO_DEVICE_MOUSE) {
        if (id == RETRO_DEVICE_ID_MOUSE_X) {
            return mouse_x;
        } else if (id == RETRO_DEVICE_ID_MOUSE_Y) {
            return mouse_y;
        } else if (id == RETRO_DEVICE_ID_MOUSE_LEFT) {
            return mouse_l;
        } else if (id == RETRO_DEVICE_ID_MOUSE_RIGHT) {
            return mouse_r;
        }
    } else if (device == RETRO_DEVICE_JOYPAD) {
        #if defined(__SWITCH__)
        GLFW_GAMEPAD_TO_RETRO(RETRO_DEVICE_ID_JOYPAD_UP, GLFW_GAMEPAD_BUTTON_DPAD_UP);
        GLFW_GAMEPAD_TO_RETRO(RETRO_DEVICE_ID_JOYPAD_DOWN, GLFW_GAMEPAD_BUTTON_DPAD_DOWN);
        GLFW_GAMEPAD_TO_RETRO(RETRO_DEVICE_ID_JOYPAD_LEFT, GLFW_GAMEPAD_BUTTON_DPAD_LEFT);
        GLFW_GAMEPAD_TO_RETRO(RETRO_DEVICE_ID_JOYPAD_RIGHT, GLFW_GAMEPAD_BUTTON_DPAD_RIGHT);
        GLFW_GAMEPAD_TO_RETRO(RETRO_DEVICE_ID_JOYPAD_L, GLFW_GAMEPAD_BUTTON_LEFT_BUMPER);
        GLFW_GAMEPAD_TO_RETRO(RETRO_DEVICE_ID_JOYPAD_R, GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER);
        GLFW_GAMEPAD_TO_RETRO(RETRO_DEVICE_ID_JOYPAD_L3, GLFW_GAMEPAD_BUTTON_LEFT_THUMB);
        GLFW_GAMEPAD_TO_RETRO(RETRO_DEVICE_ID_JOYPAD_R3, GLFW_GAMEPAD_BUTTON_RIGHT_THUMB);
        GLFW_GAMEPAD_TO_RETRO(RETRO_DEVICE_ID_JOYPAD_A, GLFW_GAMEPAD_BUTTON_A);
        GLFW_GAMEPAD_TO_RETRO(RETRO_DEVICE_ID_JOYPAD_B, GLFW_GAMEPAD_BUTTON_B);
        GLFW_GAMEPAD_TO_RETRO(RETRO_DEVICE_ID_JOYPAD_X, GLFW_GAMEPAD_BUTTON_X);
        GLFW_GAMEPAD_TO_RETRO(RETRO_DEVICE_ID_JOYPAD_Y, GLFW_GAMEPAD_BUTTON_Y);
        GLFW_GAMEPAD_TO_RETRO(RETRO_DEVICE_ID_JOYPAD_SELECT, GLFW_GAMEPAD_BUTTON_BACK);
        GLFW_GAMEPAD_TO_RETRO(RETRO_DEVICE_ID_JOYPAD_START, GLFW_GAMEPAD_BUTTON_START);
        
        if (id == RETRO_DEVICE_ID_JOYPAD_L2) {
            return (glfw_gamepad_state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] + 1) / 2;
        }
        if (id == RETRO_DEVICE_ID_JOYPAD_R2) {
            return (glfw_gamepad_state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] + 1) / 2;
        }
        #else
        GLFW_KEY_TO_RETRO(RETRO_DEVICE_ID_JOYPAD_UP, GLFW_KEY_UP);
        GLFW_KEY_TO_RETRO(RETRO_DEVICE_ID_JOYPAD_DOWN, GLFW_KEY_DOWN);
        GLFW_KEY_TO_RETRO(RETRO_DEVICE_ID_JOYPAD_LEFT, GLFW_KEY_LEFT);
        GLFW_KEY_TO_RETRO(RETRO_DEVICE_ID_JOYPAD_RIGHT, GLFW_KEY_RIGHT);
        GLFW_KEY_TO_RETRO(RETRO_DEVICE_ID_JOYPAD_L, GLFW_KEY_Q);
        GLFW_KEY_TO_RETRO(RETRO_DEVICE_ID_JOYPAD_R, GLFW_KEY_E);
        GLFW_KEY_TO_RETRO(RETRO_DEVICE_ID_JOYPAD_L2, GLFW_KEY_Z);
        GLFW_KEY_TO_RETRO(RETRO_DEVICE_ID_JOYPAD_R2, GLFW_KEY_C);
        GLFW_KEY_TO_RETRO(RETRO_DEVICE_ID_JOYPAD_A, GLFW_KEY_A);
        GLFW_KEY_TO_RETRO(RETRO_DEVICE_ID_JOYPAD_B, GLFW_KEY_B);
        GLFW_KEY_TO_RETRO(RETRO_DEVICE_ID_JOYPAD_X, GLFW_KEY_X);
        GLFW_KEY_TO_RETRO(RETRO_DEVICE_ID_JOYPAD_Y, GLFW_KEY_Y);
        #endif
    } else if (device == RETRO_DEVICE_ANALOG && index == RETRO_DEVICE_INDEX_ANALOG_LEFT && id == RETRO_DEVICE_ID_ANALOG_X) {
        return glfw_gamepad_state.axes[GLFW_GAMEPAD_AXIS_LEFT_X] * 0xFFFF / 2;
    } else if (device == RETRO_DEVICE_ANALOG && index == RETRO_DEVICE_INDEX_ANALOG_LEFT && id == RETRO_DEVICE_ID_ANALOG_Y) {
        return glfw_gamepad_state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y] * 0xFFFF / 2;
    } else if (device == RETRO_DEVICE_ANALOG && index == RETRO_DEVICE_INDEX_ANALOG_RIGHT && id == RETRO_DEVICE_ID_ANALOG_X) {
        return glfw_gamepad_state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X] * 0xFFFF / 2;
    } else if (device == RETRO_DEVICE_ANALOG && index == RETRO_DEVICE_INDEX_ANALOG_RIGHT && id == RETRO_DEVICE_ID_ANALOG_Y) {
        return glfw_gamepad_state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y] * 0xFFFF / 2;
    }
    return 0;
}

#include "CryptoManager.hpp"

int main(int argc, const char * argv[]) {
    input_state_cb = glfw_input_state_cb;
    
    glfwInit();
    
    glfwSetErrorCallback([](int i, const char *error) {
        printf("GLFW error: %s\n", error);
    });
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Test", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    
    #ifdef __SWITCH__
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    #else
    rglgen_resolve_symbols(glfwGetProcAddress);
    #endif
    
    glfwSetCursorPosCallback(window, [](GLFWwindow *w, double x, double y) {
        mouse_x = x;
        mouse_y = y;
    });

    glfwSetMouseButtonCallback(window, [](GLFWwindow *w, int button, int action, int modifiers) {
        if (button == 0) {
            mouse_l = action;
        } else if (button == 1) {
            mouse_r = action;
        }
    });
    
    glfwSetScrollCallback(window, [](GLFWwindow *w, double x, double y) {
        nanogui::scroll_callback_event(x, y);
    });
    
    glfwSetKeyCallback(window, [](GLFWwindow *w, int key, int scancode, int action, int mods) {
        glfw_keyboard_state[key] = action != GLFW_RELEASE;
        nanogui::keyboard_event(key, scancode, action, mods);
    });
    
    int width, height, fb_width, fb_height;
    glfwGetWindowSize(window, &width, &height);
    glfwGetFramebufferSize(window, &fb_width, &fb_height);
    
    #ifdef __SWITCH__
    Settings::settings()->set_working_dir("sdmc:/switch/moonlight");
    #else
    Settings::settings()->set_working_dir("/Users/rock88/Documents/RetroArch/system/moonlight");
    #endif
    
    nanogui::init();
    nanogui::ref<Application> app = new Application(Size(width, height), Size(fb_width, fb_height));
    
    nanogui::setup(1.0 / 15.0);
    
    while (!glfwWindowShouldClose(window) && !moonlight_exit) {
        #ifdef __SWITCH__
        hidScanInput();
        if (hidKeysDown(CONTROLLER_P1_AUTO) & KEY_PLUS) {
            nanogui::keyboard_event(GLFW_KEY_ESCAPE, 0, 1, 0);
        }
        #endif
        
        glfwPollEvents();
        glfwGetGamepadState(GLFW_JOYSTICK_1, &glfw_gamepad_state);
        
        InputController::controller()->handle_input(width, height);
        
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        
        nanogui::draw();
        
        glfwSwapBuffers(window);
    }
    
    nanogui::leave();
    nanogui::shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
