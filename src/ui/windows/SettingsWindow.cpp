#include "SettingsWindow.hpp"
#include "Settings.hpp"

using namespace nanogui;

SettingsWindow::SettingsWindow(nanogui::Widget* parent): ContentWindow(parent, "Settings") {
    set_left_pop_button();
    container()->set_layout(new GroupLayout(10, 10));
    
    container()->add<Label>("Resolution");
    std::vector<std::string> resolutions = { "720p", "1080p" };
    auto resolution_combo_box = container()->add<ComboBox>(resolutions);
    resolution_combo_box->set_fixed_width(200);
    resolution_combo_box->popup()->set_fixed_width(200);
    resolution_combo_box->set_callback([](auto value) {
        switch (value) {
            case 0:
                Settings::settings()->set_resolution(720);
                break;
            case 1:
                Settings::settings()->set_resolution(1080);
                break;
            default:
                break;
        }
    });
    container()->add<Widget>()->set_fixed_height(20);
    
    switch (Settings::settings()->resolution()) {
        case 720:
            resolution_combo_box->set_selected_index(0);
            break;
        case 1080:
            resolution_combo_box->set_selected_index(1);
            break;
        default:
            break;
    }
    
    container()->add<Label>("FPS");
    std::vector<std::string> fps = { "30", "60" };
    auto fps_combo_box = container()->add<ComboBox>(fps);
    fps_combo_box->set_fixed_width(200);
    fps_combo_box->popup()->set_fixed_width(200);
    fps_combo_box->set_callback([](auto value) {
        switch (value) {
            case 0:
                Settings::settings()->set_fps(30);
                break;
            case 1:
                Settings::settings()->set_fps(60);
                break;
            default:
                break;
        }
    });
    container()->add<Widget>()->set_fixed_height(20);
    
    switch (Settings::settings()->fps()) {
        case 30:
            fps_combo_box->set_selected_index(0);
            break;
        case 60:
            fps_combo_box->set_selected_index(1);
            break;
        default:
            break;
    }
    
    char bitrate_str[100];
    sprintf(bitrate_str, "Video bitrate: %0.1f Mbps", float(Settings::settings()->bitrate()) / 1000);
    
    auto video_bitrate_label = container()->add<Label>(bitrate_str);
    auto video_bitrate_slider = container()->add<Slider>();
    video_bitrate_slider->set_highlight_color(Color(62, 78, 184, 255));
    video_bitrate_slider->set_range({0.5, 30});
    video_bitrate_slider->set_value(float(Settings::settings()->bitrate()) / 1000);
    video_bitrate_slider->set_fixed_size(Size(200, 30));
    video_bitrate_slider->set_callback([video_bitrate_label](auto value) {
        float bitrate = round(2.0 * value) / 2.0;
        char bitrate_str[100];
        sprintf(bitrate_str, "Video bitrate: %0.1f Mbps", bitrate);
        video_bitrate_label->set_caption(bitrate_str);
        Settings::settings()->set_bitrate(bitrate * 1000);
    });
    container()->add<Widget>()->set_fixed_height(20);
    
    container()->add<Label>("Input Settings");
    auto swap_input = container()->add<CheckBox>("Swap A/B and X/Y");
    swap_input->set_checked(Settings::settings()->swap_ab_xy());
    swap_input->set_callback([](auto value) {
        Settings::settings()->set_swap_ab_xy(value);
    });
}

void SettingsWindow::window_disappear() {
    Settings::settings()->save();
}
