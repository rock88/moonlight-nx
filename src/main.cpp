#include <stdio.h>
#include <stdlib.h>
#include "Application.hpp"
#include "Settings.hpp"
#include "Limelight.h"
#include "GameStreamClient.hpp"
#include "Logger.hpp"
#include "MouseController.hpp"
#include "SwitchMouseFrontend.hpp"
#include "KeyboardController.hpp"
#include "SwitchKeyboardFrontend.hpp"
#include "GamepadController.hpp"
#include "SwitchGamepadFrontend.hpp"
#include "MoonlightSession.hpp"
#include "SwitchMoonlightSessionDecoderAndRenderProvider.hpp"
#include <glad/glad.h>
#include <switch.h>
#include <GLFW/glfw3.h>

int moonlight_exit = 0;

static int m_width, m_height, m_fb_width, m_fb_height;

int main(int argc, const char * argv[]) {
    glfwInit();
    
    #ifdef __SWITCH__
    appletSetFocusHandlingMode(AppletFocusHandlingMode_NoSuspend);
    
    bool recording_supported = false;
    appletIsGamePlayRecordingSupported(&recording_supported);
    
    if (recording_supported) {
        appletInitializeGamePlayRecording();
    }
    
    Settings::instance().set_working_dir("sdmc:/switch/moonlight");
    #else
    Settings::instance().set_working_dir("/Users/Shared/moonlight-nx");
    #endif
    
    Logger::info("Moonlight", "Starting...");
    
    glfwSetErrorCallback([](int i, const char *error) {
        Logger::error("GLFW", "error: %s", error);
    });
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Test", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    
    glfwGetWindowSize(window, &m_width, &m_height);
    glfwGetFramebufferSize(window, &m_fb_width, &m_fb_height);
    
    glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        m_width = width;
        m_height = height;
        glfwGetFramebufferSize(window, &m_fb_width, &m_fb_height);
        nanogui::window_resize_callback_event(width, height, m_fb_width, m_fb_height);
    });
    
    GameStreamClient::instance().start();
    
    MoonlightSession::set_provider(new SwitchMoonlightSessionDecoderAndRenderProvider());
    MouseController::instance().init(new SwitchMouseFrontend(window));
    KeyboardController::instance().init(new SwitchKeyboardFrontend(window));
    GamepadController::instance().init(new SwitchGamepadFrontend());
    
    nanogui::init();
    nanogui::ref<Application> app = new Application({ (float)m_width, (float)m_height }, { (float)m_fb_width, (float)m_fb_height });
    
    nanogui::setup(1.0 / 15.0);
    
    while (!glfwWindowShouldClose(window) && !moonlight_exit) {
        glfwPollEvents();
        
        MouseController::instance().handle_mouse();
        KeyboardController::instance().handle_keyboard();
        GamepadController::instance().handle_gamepad();
        
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        
        nanogui::draw();
        
        MouseController::instance().draw_cursor(app);
        
        glfwSwapBuffers(window);
    }
    
    GameStreamClient::instance().stop();
    nanogui::leave();
    nanogui::shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
