#include "AppButton.hpp"
#include "Application.hpp"
#include "GameStreamClient.hpp"
#include "BoxArtManager.hpp"
#include "nanovg.h"

using namespace nanogui;

AppButton::AppButton(Widget* parent, const std::string &address, APP_LIST app, int current_game): Button(parent, "") {
    m_address = address;
    m_app = app;
    
    set_fixed_size(Size(210, 296));
    set_layout(new BoxLayout(Orientation::Vertical, Alignment::Middle));
    
    m_label = add<Label>(m_app.name);
    m_label->set_fixed_width(fixed_width() - 12);
    
    if (m_app.id == current_game) {
        m_label->set_caption(m_label->caption() + " (Running)");
    }
    
    if (!BoxArtManager::manager()->has_boxart(m_app.id)) {
        inc_ref();
        GameStreamClient::client()->app_boxart(m_address, m_app.id, [this](auto result) {
            if (result.isSuccess()) {
                BoxArtManager::manager()->set_data(result.value(), m_app.id);
            }
            
            dec_ref();
        });
    }
}

void AppButton::draw(NVGcontext *ctx) {
    int handle = BoxArtManager::manager()->texture_id(m_app.id);
    
    if (handle == -1 && BoxArtManager::manager()->has_boxart(m_app.id)) {
        BoxArtManager::manager()->make_texture_from_boxart(ctx, m_app.id);
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
        nvgRect(ctx, m_pos.x(), m_pos.y(), width(), m_label->height());
        nvgFill(ctx);
        nvgRestore(ctx);
        
        Widget::draw(ctx);
    }
}
