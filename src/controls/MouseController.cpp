#include "MouseController.hpp"
#include "Application.hpp"
#include <nanogui/opengl.h>
#include <GLFW/glfw3.h>

void MouseController::init(GLFWwindow* window) {
    hidInitializeMouse();
    hidInitializeTouchScreen();
    
    glfwSetCursorPosCallback(window, [](auto _, double x, double y) {
        MouseController::instance().handle_mouse_move(x, y);
    });
    
    glfwSetMouseButtonCallback(window, [](auto _, int button, int action, int modifiers) {
        MouseController::instance().handle_mouse_buttons(button, action, modifiers);
    });
    
    glfwSetScrollCallback(window, [](auto _, double x, double y) {
        MouseController::instance().handle_mouse_scroll(x, y);
    });
}

void MouseController::handle_mouse() {
    HidMouseState hid_mouse_state;
    
    if (hidGetMouseStates(&hid_mouse_state, 1) && (hid_mouse_state.attributes & HidMouseAttribute_IsConnected)) {
        m_has_hid_mouse = true;
        
        if (m_hid_mouse_state.x != hid_mouse_state.x || m_hid_mouse_state.y != hid_mouse_state.y || m_hid_mouse_state.buttons != hid_mouse_state.buttons || m_hid_mouse_state.wheel_delta_x != hid_mouse_state.wheel_delta_x) {
            m_draw_cursor_for_hid_mouse = true;
            m_hid_mouse_state = hid_mouse_state;
            
            MouseState state;
            state.x = hid_mouse_state.x;
            state.y = hid_mouse_state.y;
            state.l_pressed = hid_mouse_state.buttons & HidMouseButton_Left;
            state.r_pressed = hid_mouse_state.buttons & HidMouseButton_Right;
            state.scroll_y = (double)hid_mouse_state.wheel_delta_x / 100; // Why wheel_delta_x?
            
            if (m_state.x != state.x || m_state.y != state.y) {
                nanogui::cursor_pos_callback_event(state.x, state.y);
            }
            
            if (m_state.l_pressed != state.l_pressed) {
                nanogui::mouse_button_callback_event(NANOGUI_MOUSE_BUTTON_1, state.l_pressed ? NANOGUI_PRESS : NANOGUI_RELEASE, 0);
            }
            
            if (m_state.r_pressed != state.r_pressed) {
                nanogui::mouse_button_callback_event(NANOGUI_MOUSE_BUTTON_2, state.r_pressed ? NANOGUI_PRESS : NANOGUI_RELEASE, 0);
            }
            
            if (m_state.scroll_y != state.scroll_y) {
                nanogui::scroll_callback_event(0, state.scroll_y);
            }
            
            set_new_mouse_state(state);
        }
    } else {
        m_has_hid_mouse = false;
    }
    
    // Simulate mouse wheel with two finger movements...
    HidTouchScreenState hid_touch_screen_state;
    if (hidGetTouchScreenStates(&hid_touch_screen_state, 1) && hid_touch_screen_state.count == 2) {
        double y = (hid_touch_screen_state.touches[0].y + hid_touch_screen_state.touches[1].y) / 2;
        
        if (m_last_touch_y != 0 && m_last_touch_y != y) {
            auto state = m_state;
            state.scroll_y = (m_last_touch_y - y) / 10;
            
            if (set_new_mouse_state(state)) {
                // Invert scroll_y for make scroll line on an iOS/Android :C
                nanogui::scroll_callback_event(0, -state.scroll_y);
            }
        }
        
        m_last_touch_y = y;
    } else {
        m_last_touch_y = 0;
    }
}

void MouseController::draw_cursor(Application *app) {
    if (m_has_hid_mouse && m_draw_cursor_for_hid_mouse) {
        auto ctx = app->nvg_context();
        nvgSave(ctx);
        nvgReset(ctx);
        
        nvgFillColor(ctx, nanogui::Color(255, 255, 255, 245));
        nvgFontSize(ctx, 20);
        nvgFontFace(ctx, "icons");
        nvgTextAlign(ctx, NVG_ALIGN_TOP | NVG_ALIGN_MIDDLE);
        nvgText(ctx, m_state.x, m_state.y, nanogui::utf8(FA_MOUSE_POINTER).data(), NULL);
        
        nvgRestore(ctx);
        
        app->nvg_flush();
    }
}

void MouseController::handle_mouse_move(int x, int y) {
    m_draw_cursor_for_hid_mouse = false;
    
    auto state = m_state;
    state.x = x;
    state.y = y;
    
    if (set_new_mouse_state(state)) {
        nanogui::cursor_pos_callback_event(x, y);
    }
}

void MouseController::handle_mouse_buttons(int button, int action, int modifiers) {
    m_draw_cursor_for_hid_mouse = false;
    
    auto state = m_state;
    
    if (button == NANOGUI_MOUSE_BUTTON_1) {
        state.l_pressed = action == NANOGUI_PRESS;
    } else if (button == NANOGUI_MOUSE_BUTTON_2) {
        state.r_pressed = action == NANOGUI_PRESS;
    }
    
    if (set_new_mouse_state(state)) {
        nanogui::mouse_button_callback_event(button, action, modifiers);
    }
}

void MouseController::handle_mouse_scroll(int x, int y) {
    m_draw_cursor_for_hid_mouse = false;
    
    auto state = m_state;
    state.scroll_y = y;
    
    if (set_new_mouse_state(state)) {
        nanogui::scroll_callback_event(x, y);
    }
}

bool MouseController::set_new_mouse_state(MouseState state) {
    if (m_state.x != state.x || m_state.y != state.y || m_state.l_pressed != state.l_pressed || m_state.r_pressed != state.r_pressed || m_state.scroll_y != state.scroll_y) {
        m_state = state;
        m_is_dirty = true;
        //printf("New state: %ix%i, L: %i, R: %i\n", state.x, state.y, state.l_pressed, state.r_pressed);
    } else {
        m_is_dirty = false;
    }
    return m_is_dirty;
}
