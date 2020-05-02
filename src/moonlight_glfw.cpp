#include <stdio.h>
#include <stdlib.h>
#include <GLFW/glfw3.h>
#include "glsym/glsym.h"
#include "Application.hpp"
#include "Server.hpp"

int main(int argc, const char * argv[]) {
    glfwInit();
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Test", NULL, NULL);
    glfwMakeContextCurrent(window);
    rglgen_resolve_symbols(glfwGetProcAddress);
    glfwSwapInterval(1);
    
    glfwSetCursorPosCallback(window, [](GLFWwindow *w, double x, double y) {
        nanogui::cursor_pos_callback_event(x, y);
    });

    glfwSetMouseButtonCallback(window, [](GLFWwindow *w, int button, int action, int modifiers) {
        nanogui::mouse_button_callback_event(button, action, modifiers);
    });
    
    glfwSetScrollCallback(window, [](GLFWwindow *w, double x, double y) {
        nanogui::scroll_callback_event(x, y);
    });
    
    int width, height, fb_width, fb_height;
    glfwGetWindowSize(window, &width, &height);
    glfwGetFramebufferSize(window, &fb_width, &fb_height);
    
    Server::server()->set_working_dir("/Users/rock88/Documents/RetroArch/system");
    
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
