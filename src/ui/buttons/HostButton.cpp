#include "HostButton.hpp"
#include "Server.hpp"

using namespace nanogui;

HostButton::HostButton(Widget* parent, const std::string &host): Button(parent, "", FA_POWER_OFF) {
    m_host = host;
    m_data.paired = false;
    set_icon_extra_scale(2);
    set_icon_position(IconPosition::LeftCentered);
    
    set_layout(new BoxLayout(Orientation::Vertical, Alignment::Middle));
    add<Widget>()->set_fixed_height(80);
    add<Label>(host);
    
    LOG("Try connect to %s\n", host.c_str());
    
    Server::server()->connect(host, [this](auto result) {
        if (result.isSuccess()) {
            m_data = result.value();
            m_data.serverInfo.address = m_host.c_str();
            
            if (m_data.paired) {
                set_icon(FA_DESKTOP);
            } else {
                set_icon(FA_QUESTION);
            }
        }
    });
}

void HostButton::draw(NVGcontext *ctx) {
    Button::draw(ctx);
    Widget::draw(ctx);
}
