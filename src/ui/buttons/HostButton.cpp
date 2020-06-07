#include "HostButton.hpp"
#include "GameStreamClient.hpp"
#include "nanovg.h"
#include <memory>

using namespace nanogui;

HostButton::HostButton(Widget* parent, const std::string &address): Button(parent, "") {
    m_address = address;
    m_host_status_icon = FA_QUESTION;
    
    set_layout(new BoxLayout(Orientation::Vertical, Alignment::Middle));
    add<Widget>()->set_fixed_height(170);
    m_label = add<Label>(m_address);
    m_label->set_font_size(20);
    
    inc_ref();
    GameStreamClient::client()->connect(m_address, [this](auto result) {
        if (result.isSuccess()) {
            m_label->set_caption(result.value().hostname ?: m_address);
            m_is_active = true;
            m_is_paired = result.value().paired;
            m_host_status_icon = m_is_paired ? FA_CHECK : FA_QUESTION;
        } else {
            m_is_paired = false;
            m_is_active = false;
            m_host_status_icon = FA_POWER_OFF;
        }
        
        if (ref_count() > 1) {
            if (auto scr = screen()) {
                scr->perform_layout();
            }
        }
        
        dec_ref();
    });
}

void HostButton::draw(NVGcontext *ctx) {
    Button::draw(ctx);
    
    nvgSave(ctx);
    nvgReset(ctx);
    
    nvgTranslate(ctx, absolute_position().x(), absolute_position().y());
    nvgFillColor(ctx, Color(255, 255, 255, 255));
    nvgFontSize(ctx, 130);
    nvgFontFace(ctx, "icons");
    nvgTextAlign(ctx, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
    nvgText(ctx, width() / 2, height() / 2 - 4, utf8(FA_DESKTOP).data(), NULL);
    
    nvgFontSize(ctx, 40);
    nvgText(ctx, width() / 2, height() / 2 - 16, utf8(m_host_status_icon).data(), NULL);
    
    nvgRestore(ctx);
}
