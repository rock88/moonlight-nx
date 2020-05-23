#include <stdio.h>
#include <stdlib.h>
#include "Application.hpp"
#include "Settings.hpp"
#include "Limelight.h"
#include "InputController.hpp"
#include <glad/glad.h>

#ifdef __SWITCH__
#include <switch.h>
#endif

#include <GLFW/glfw3.h>

GLFWgamepadstate glfw_gamepad_state;
volatile int moonlight_exit = 0;

int width, height, fb_width, fb_height;

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
    glfwSwapInterval(1);
    
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    
    glfwGetWindowSize(window, &width, &height);
    glfwGetFramebufferSize(window, &fb_width, &fb_height);
    
    glfwSetCursorPosCallback(window, [](GLFWwindow *w, double x, double y) {
        InputController::controller()->handle_cursor_event(width, height, x, y);
    });
    
    glfwSetMouseButtonCallback(window, [](GLFWwindow *w, int button, int action, int modifiers) {
        InputController::controller()->handle_mouse_event(button, action, modifiers);
    });
    
    glfwSetScrollCallback(window, [](GLFWwindow *w, double x, double y) {
        nanogui::scroll_callback_event(x, y);
    });
    
    glfwSetKeyCallback(window, [](GLFWwindow *w, int key, int scancode, int action, int mods) {
        InputController::controller()->handle_keyboard_event(key, scancode, action, mods);
    });
    
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
        
        #ifdef __SWITCH__
        glfwGetGamepadState(GLFW_JOYSTICK_1, &glfw_gamepad_state);
        InputController::controller()->handle_gamepad_event(&glfw_gamepad_state);
        #endif
        
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        
        nanogui::draw();
        
        glfwSwapBuffers(window);
    }
    
    nanogui::leave();
    //nanogui::shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
