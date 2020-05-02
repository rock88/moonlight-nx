#include "HostButton.hpp"
#include "Server.hpp"

using namespace nanogui;

HostButton::HostButton(Widget* parent, const std::string &host): Button(parent, "", FA_QUESTION) {
    m_host = host;
    m_data.paired = false;
    set_icon_extra_scale(2);
    set_icon_position(IconPosition::LeftCentered);
    
    set_layout(new BoxLayout(Orientation::Vertical, Alignment::Middle));
    add<Widget>()->set_fixed_height(80);
    add<Label>(host);
    
    Server::server()->connect(host, [this](auto result) {
        if (result.isSuccess()) {
            m_data = *result.value();
            
            if (m_data.paired) {
                set_icon(FA_CHECK);
            } else {
                set_icon(FA_EXCLAMATION);
            }
        }
    });
}

void HostButton::draw(NVGcontext *ctx) {
    Button::draw(ctx);
    Widget::draw(ctx);
}
