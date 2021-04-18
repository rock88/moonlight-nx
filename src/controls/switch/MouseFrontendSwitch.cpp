#include "MouseFrontendSwitch.hpp"
#include <nanogui/opengl.h>
#include <GLFW/glfw3.h>

static MouseFrontendSwitch* m_frontend = NULL;

MouseFrontendSwitch::MouseFrontendSwitch(GLFWwindow* window) {
    m_frontend = this;
    
    hidInitializeMouse();
    hidInitializeTouchScreen();
    
    glfwSetCursorPosCallback(window, [](auto _, double x, double y) {
        m_frontend->handle_mouse_move(x, y);
    });
    
    glfwSetMouseButtonCallback(window, [](auto _, int button, int action, int modifiers) {
        m_frontend->handle_mouse_buttons(button, action, modifiers);
    });
    
    glfwSetScrollCallback(window, [](auto _, double x, double y) {
        m_frontend->handle_mouse_scroll(x, y);
    });
}

void MouseFrontendSwitch::handle_mouse() {
    HidMouseState hid_mouse_state;
    
    if (hidGetMouseStates(&hid_mouse_state, 1) && (hid_mouse_state.attributes & HidMouseAttribute_IsConnected)) {
        if (m_hid_mouse_state.x != hid_mouse_state.x || m_hid_mouse_state.y != hid_mouse_state.y || m_hid_mouse_state.buttons != hid_mouse_state.buttons || m_hid_mouse_state.wheel_delta_x != hid_mouse_state.wheel_delta_x) {
            m_hid_mouse_is_used = true;
            m_hid_mouse_state = hid_mouse_state;
            
            m_mouse_state.x = hid_mouse_state.x;
            m_mouse_state.y = hid_mouse_state.y;
            m_mouse_state.l_pressed = hid_mouse_state.buttons & HidMouseButton_Left;
            m_mouse_state.m_pressed = hid_mouse_state.buttons & HidMouseButton_Middle;
            m_mouse_state.r_pressed = hid_mouse_state.buttons & HidMouseButton_Right;
            m_mouse_state.scroll_y = (double)hid_mouse_state.wheel_delta_x / 100; // Why wheel_delta_x?
        }
    } else {
        m_hid_mouse_is_used = false;
    }
    
    // Simulate mouse wheel with two finger movements...
    HidTouchScreenState hid_touch_screen_state;
    if (hidGetTouchScreenStates(&hid_touch_screen_state, 1) && hid_touch_screen_state.count == 2) {
        double y = (hid_touch_screen_state.touches[0].y + hid_touch_screen_state.touches[1].y) / 2;
        
        if (m_last_touch_y != 0 && m_last_touch_y != y) {
            m_mouse_state.scroll_y = (m_last_touch_y - y) / 10;
        }
        
        m_last_touch_y = y;
    } else {
        m_last_touch_y = 0;
    }
}

void MouseFrontendSwitch::handle_mouse_move(double x, double y) {
    m_hid_mouse_is_used = false;
    
    m_mouse_state.x = (int)x;
    m_mouse_state.y = (int)y;
}

void MouseFrontendSwitch::handle_mouse_buttons(int button, int action, int modifiers) {
    m_hid_mouse_is_used = false;
    
    if (button == NANOGUI_MOUSE_BUTTON_1) {
        m_mouse_state.l_pressed = action == NANOGUI_PRESS;
    } else if (button == NANOGUI_MOUSE_BUTTON_2) {
        m_mouse_state.r_pressed = action == NANOGUI_PRESS;
    }
}

void MouseFrontendSwitch::handle_mouse_scroll(int x, int y) {
    m_hid_mouse_is_used = false;
    
    m_mouse_state.scroll_y = y;
}
