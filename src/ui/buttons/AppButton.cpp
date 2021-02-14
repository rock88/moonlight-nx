#include "AppButton.hpp"
#include "Application.hpp"
#include "GameStreamClient.hpp"
#include "BoxArtManager.hpp"
#include "InputSettingsWindow.hpp"
#include "nanovg.h"
#include <nanogui/opengl.h>

using namespace nanogui;

AppButton::AppButton(Widget* parent, const std::string &address, AppInfo app, int current_game): Button(parent, "") {
    m_address = address;
    m_app = app;
    
    m_label = add<Label>(m_app.name);
    
    set_fixed_size(Size(210, 296));
    set_layout(new BoxLayout(Orientation::Vertical, Alignment::Minimum, 10));
    
    if (m_app.app_id == current_game) {
        m_label->set_caption(m_label->caption() + " (Running)");
    }
    
    if (!BoxArtManager::instance().has_boxart(m_app.app_id)) {
        inc_ref();
        GameStreamClient::instance().app_boxart(m_address, m_app.app_id, [this](auto result) {
            if (result.isSuccess()) {
                BoxArtManager::instance().set_data(result.value(), m_app.app_id);
            }
            
            dec_ref();
        });
    }
}

bool AppButton::gamepad_button_event(int jid, int button, int action) {
    if (action && button == NANOGUI_GAMEPAD_BUTTON_Y) {
        if (auto application = dynamic_cast<Application *>(screen())) {
            application->push_window<InputSettingsWindow>(m_app.app_id, m_app.name);
        }
        return true;
    }
    return Button::gamepad_button_event(jid, button, action);
}

void AppButton::set_fixed_size(const Vector2i &fixed_size) {
    m_label->set_fixed_width(fixed_size.x() - 10);
    
    Button::set_fixed_size(fixed_size);
}

void AppButton::draw(NVGcontext *ctx) {
    int handle = BoxArtManager::instance().texture_id(m_app.app_id);
    
    if (handle == -1 && BoxArtManager::instance().has_boxart(m_app.app_id)) {
        BoxArtManager::instance().make_texture_from_boxart(ctx, m_app.app_id);
    }
    
    if (handle != -1) {
        nvgSave(ctx);
        
        int w, h;
        nvgImageSize(ctx, handle, &w, &h);
        
        nvgBeginPath(ctx);
        nvgTranslate(ctx, m_pos.x(), m_pos.y());
        NVGpaint paint = nvgImagePattern(ctx, 0, 0, fixed_width(), fixed_height(), 0, handle, 1);
        nvgRect(ctx, 0, 0, fixed_width(), fixed_height());
        nvgFillPaint(ctx, paint);
        nvgFill(ctx);
        nvgRestore(ctx);
    }
    
    Button::draw(ctx);
    
    if (m_label->visible()) {
        nvgSave(ctx);
        nvgFillColor(ctx, Color(0, 0, 0, 200));
        nvgBeginPath(ctx);
        nvgRect(ctx, m_pos.x(), m_pos.y(), width(), m_label->height() + 20);
        nvgFill(ctx);
        nvgRestore(ctx);
        
        Widget::draw(ctx);
    }
}
