#include <stdio.h>
#include <stdlib.h>
#include <GLFW/glfw3.h>
#include "glsym/glsym.h"
#include "Application.hpp"
#include "Settings.hpp"
#include "Limelight.h"
#include "moonlight_libretro_wrapper.h"
#include "gl_render.h"
#include "libretro.h"

int main(int argc, const char * argv[]) {
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
    rglgen_resolve_symbols(glfwGetProcAddress);
    glfwSwapInterval(1);
    
    gl_render_init();
    
    glfwSetCursorPosCallback(window, [](GLFWwindow *w, double x, double y) {
        nanogui::cursor_pos_callback_event(x, y);
    });

    glfwSetMouseButtonCallback(window, [](GLFWwindow *w, int button, int action, int modifiers) {
        nanogui::mouse_button_callback_event(button, action, modifiers);
    });
    
    glfwSetScrollCallback(window, [](GLFWwindow *w, double x, double y) {
        nanogui::scroll_callback_event(x, y);
    });
    
    glfwSetKeyCallback(window, [](GLFWwindow *w, int key, int scancode, int action, int mods) {
        if (GLFW_KEY_A <= key && key <= GLFW_KEY_Z) {
            keyboard_state[RETROK_a + key - GLFW_KEY_A] = action == GLFW_PRESS;
        }
        
        keyboard_state[RETROK_w] ? game_pad_state.buttonFlags |= UP_FLAG : game_pad_state.buttonFlags &= ~UP_FLAG;
        keyboard_state[RETROK_s] ? game_pad_state.buttonFlags |= DOWN_FLAG : game_pad_state.buttonFlags &= ~DOWN_FLAG;
    });
    
    int width, height, fb_width, fb_height;
    glfwGetWindowSize(window, &width, &height);
    glfwGetFramebufferSize(window, &fb_width, &fb_height);
    
    Settings::settings()->set_working_dir("/Users/rock88/Documents/RetroArch/system");
    
    nanogui::init();
    nanogui::ref<Application> app = new Application(Size(width, height), Size(fb_width, fb_height));
    
    nanogui::setup(1.0 / 60.0 * 1000);
    
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        
        nanogui::draw();
        
        glfwSwapBuffers(window);
    }
    return 0;
}
