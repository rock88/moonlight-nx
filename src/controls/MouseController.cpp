#include "MouseController.hpp"
#include "Application.hpp"
#include <nanogui/opengl.h>

void MouseController::init(MouseFrontend* frontend) {
    m_frontend = frontend;
}

void MouseController::handle_mouse() {
    m_frontend->handle_mouse();
    
    auto state = m_frontend->mouse_state();
    
    if (m_mouse_state.x != state.x || m_mouse_state.y != state.y) {
        nanogui::cursor_pos_callback_event((double)state.x, (double)state.y);
    }
    
    if (m_mouse_state.l_pressed != state.l_pressed) {
        nanogui::mouse_button_callback_event(NANOGUI_MOUSE_BUTTON_1, state.l_pressed ? NANOGUI_PRESS : NANOGUI_RELEASE, 0);
    }
    
    if (m_mouse_state.r_pressed != state.r_pressed) {
        nanogui::mouse_button_callback_event(NANOGUI_MOUSE_BUTTON_2, state.r_pressed ? NANOGUI_PRESS : NANOGUI_RELEASE, 0);
    }
    
    if (m_mouse_state.scroll_y != state.scroll_y) {
        nanogui::scroll_callback_event(0, state.scroll_y);
    }
    
    m_mouse_state = state;
}

void MouseController::draw_cursor(Application *app) {
    if (m_frontend->hid_mouse_is_used() && m_draw_cursor_for_hid_mouse) {
        auto ctx = app->nvg_context();
        nvgSave(ctx);
        nvgReset(ctx);
        
        nvgFillColor(ctx, nanogui::Color(255, 255, 255, 245));
        nvgFontSize(ctx, 20);
        nvgFontFace(ctx, "icons");
        nvgTextAlign(ctx, NVG_ALIGN_TOP | NVG_ALIGN_MIDDLE);
        nvgText(ctx, m_mouse_state.x, m_mouse_state.y, nanogui::utf8(FA_MOUSE_POINTER).data(), NULL);
        
        nvgRestore(ctx);
        
        app->nvg_flush();
    }
}
