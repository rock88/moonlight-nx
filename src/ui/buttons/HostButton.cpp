#include "HostButton.hpp"
#include "GameStreamClient.hpp"

using namespace nanogui;

HostButton::HostButton(Widget* parent, const std::string &host): Button(parent, "", FA_POWER_OFF) {
    m_host = host;
    set_icon_extra_scale(2);
    set_icon_position(IconPosition::LeftCentered);
    
    set_layout(new BoxLayout(Orientation::Vertical, Alignment::Middle));
    add<Widget>()->set_fixed_height(80);
    add<Label>(host);
    
    LOG("Try connect to %s\n", host.c_str());
    
    GameStreamClient::client()->connect(host, [this](auto result) {
        if (result.isSuccess()) {
            m_is_active = true;
            m_is_paired = result.value().paired;
            set_icon(m_is_paired ? FA_DESKTOP : FA_QUESTION);
        } else {
            m_is_paired = false;
            m_is_active = false;
            set_icon(FA_POWER_OFF);
        }
    });
}

void HostButton::draw(NVGcontext *ctx) {
    Button::draw(ctx);
    Widget::draw(ctx);
}
