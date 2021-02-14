#include "AddHostWindow.hpp"
#include "GameStreamClient.hpp"
#include "LoadingOverlay.hpp"
#include "Alert.hpp"

using namespace nanogui;

AddHostWindow::AddHostWindow(Widget *parent): ContentWindow(parent, "Add Host") {
    set_left_pop_button();
    set_box_layout(Orientation::Vertical, Alignment::Fill);
    
    auto text = container()->add<TextBox>("");
    text->set_placeholder("Enter the IP address of your GameStream PC");
    text->set_fixed_height(60);
    text->set_editable(true);
    
    std::vector<std::string> keys {"7", "8", "9", "4", "5", "6", "1", "2", "3"};
    
    auto number_buttons_container = container()->add<Widget>();
    number_buttons_container->set_fixed_width(320);
    number_buttons_container->set_layout(new GridLayout(Orientation::Horizontal, 3, Alignment::Middle, 0, 10));
    
    for (auto key: keys) {
        auto button = number_buttons_container->add<Button>(key);
        button->set_fixed_size(Size(100, 100));
        button->set_callback([text, button] {
            text->set_value(text->value() + button->caption());
        });
    }
    
    auto other_buttons_container = container()->add<Widget>();
    other_buttons_container->set_layout(new BoxLayout(Orientation::Horizontal, Alignment::Minimum, 0, 10));
    
    auto dot = other_buttons_container->add<Button>(".");
    dot->set_fixed_size(Size(100, 100));
    dot->set_callback([text, dot] {
        text->set_value(text->value() + dot->caption());
    });
    
    auto zero = other_buttons_container->add<Button>("0");
    zero->set_fixed_size(Size(100, 100));
    zero->set_callback([text, zero] {
        text->set_value(text->value() + zero->caption());
    });
    
    auto backspace = other_buttons_container->add<Button>("");
    backspace->set_icon(FA_BACKSPACE);
    backspace->set_icon_extra_scale(2);
    backspace->set_fixed_size(Size(100, 100));
    backspace->set_callback([text] {
        if (text->value().size() > 0) {
            auto value = text->value();
            value.pop_back();
            text->set_value(value);
        }
    });
    
    auto connect = other_buttons_container->add<Button>("Connect");
    connect->set_fixed_size(Size(100, 100));
    connect->set_callback([text, this] {
        if (text->value().size() > 0) {
            auto loader = add<LoadingOverlay>();
            
            GameStreamClient::client()->connect(text->value(), [this, loader](auto result) {
                loader->dispose();
                
                if (result.isSuccess()) {
                    this->pop();
                } else {
                    screen()->add<Alert>("Error", result.error());
                }
            });
        }
    });
}
