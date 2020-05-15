#include "SettingsWindow.hpp"
#include "Settings.hpp"

using namespace nanogui;

#define SET_SETTING(n, func) \
    case n: \
        Settings::settings()->func; \
        break;

#define GET_SETTINGS(combo_box, n, i) \
    case n: \
        combo_box->set_selected_index(i); \
        break;

#define DEFAULT \
    default: \
        break;

SettingsWindow::SettingsWindow(nanogui::Widget* parent): ContentWindow(parent, "Settings") {
    set_left_pop_button();
    
    set_box_layout(Orientation::Horizontal, Alignment::Minimum);
    
    int container_width = 380;
    int component_width = 200;
    
    auto left_container = container()->add<Widget>();
    left_container->set_layout(new GroupLayout(30, 10, 40, 10));
    left_container->set_fixed_width(container_width);
    
    left_container->add<Label>("Resolution");
    std::vector<std::string> resolutions = { "720p", "1080p" };
    auto resolution_combo_box = left_container->add<ComboBox>(resolutions);
    resolution_combo_box->set_fixed_width(component_width);
    resolution_combo_box->popup()->set_fixed_width(component_width);
    resolution_combo_box->set_callback([](auto value) {
        switch (value) {
            SET_SETTING(0, set_resolution(720));
            SET_SETTING(1, set_resolution(1080));
            DEFAULT;
        }
    });
    
    switch (Settings::settings()->resolution()) {
        GET_SETTINGS(resolution_combo_box, 720, 0);
        GET_SETTINGS(resolution_combo_box, 1080, 1);
        DEFAULT;
    }
    
    left_container->add<Label>("FPS");
    std::vector<std::string> fps = { "30", "60" };
    auto fps_combo_box = left_container->add<ComboBox>(fps);
    fps_combo_box->set_fixed_width(component_width);
    fps_combo_box->popup()->set_fixed_width(component_width);
    fps_combo_box->set_callback([](auto value) {
        switch (value) {
            SET_SETTING(0, set_fps(30));
            SET_SETTING(1, set_fps(60));
            DEFAULT;
        }
    });
    
    switch (Settings::settings()->fps()) {
        GET_SETTINGS(fps_combo_box, 30, 0);
        GET_SETTINGS(fps_combo_box, 60, 1);
        DEFAULT;
    }
    
    left_container->add<Label>("Video codec");
    std::vector<std::string> video_codec = { "H.264", "HEVC (H.265)" };
    auto video_codec_combo_box = left_container->add<ComboBox>(video_codec);
    video_codec_combo_box->set_fixed_width(component_width);
    video_codec_combo_box->popup()->set_fixed_width(component_width);
    video_codec_combo_box->set_callback([](auto value) {
        switch (value) {
            SET_SETTING(0, set_video_codec(H264));
            SET_SETTING(1, set_video_codec(H265));
            DEFAULT;
        }
    });
    
    switch (Settings::settings()->video_codec()) {
        GET_SETTINGS(video_codec_combo_box, H264, 0);
        GET_SETTINGS(video_codec_combo_box, H265, 1);
        DEFAULT;
    }
    
    char bitrate_str[100];
    sprintf(bitrate_str, "Video bitrate: %0.1f Mbps", float(Settings::settings()->bitrate()) / 1000);
    
    auto video_bitrate_label = left_container->add<Label>(bitrate_str);
    auto video_bitrate_slider = left_container->add<Slider>();
    video_bitrate_slider->set_highlight_color(Color(62, 78, 184, 255));
    video_bitrate_slider->set_range({0.5, 150});
    video_bitrate_slider->set_value(float(Settings::settings()->bitrate()) / 1000);
    video_bitrate_slider->set_highlighted_range({0, float(Settings::settings()->bitrate()) / 1000 / 150});
    video_bitrate_slider->set_fixed_size(Size(container_width - 80, 30));
    video_bitrate_slider->set_callback([video_bitrate_label, video_bitrate_slider](auto value) {
        video_bitrate_slider->set_highlighted_range({0, value / 150});
        float bitrate = round(2.0 * value) / 2.0;
        char bitrate_str[100];
        sprintf(bitrate_str, "Video bitrate: %0.1f Mbps", bitrate);
        video_bitrate_label->set_caption(bitrate_str);
        Settings::settings()->set_bitrate(bitrate * 1000);
    });
    
    left_container->add<Label>("Input Settings");
    auto swap_input = left_container->add<CheckBox>("Swap A/B and X/Y");
    swap_input->set_checked(Settings::settings()->swap_ab_xy());
    swap_input->set_callback([](auto value) {
        Settings::settings()->set_swap_ab_xy(value);
    });
    
    auto right_container = container()->add<Widget>();
    right_container->set_layout(new GroupLayout(30, 10));
    right_container->set_fixed_width(380);
    
    right_container->add<Label>("Decoder Threads");
    std::vector<std::string> decoder_threads = { "0 (No use threads)", "2", "3", "4" };
    auto decoder_threads_combo_box = right_container->add<ComboBox>(decoder_threads);
    decoder_threads_combo_box->set_fixed_width(component_width);
    decoder_threads_combo_box->popup()->set_fixed_width(component_width);
    decoder_threads_combo_box->set_callback([](auto value) {
        switch (value) {
            SET_SETTING(0, set_decoder_threads(0));
            SET_SETTING(1, set_decoder_threads(2));
            SET_SETTING(2, set_decoder_threads(3));
            SET_SETTING(3, set_decoder_threads(4));
            DEFAULT;
        }
    });
    
    switch (Settings::settings()->decoder_threads()) {
        GET_SETTINGS(decoder_threads_combo_box, 0, 0);
        GET_SETTINGS(decoder_threads_combo_box, 2, 1);
        GET_SETTINGS(decoder_threads_combo_box, 3, 2);
        GET_SETTINGS(decoder_threads_combo_box, 4, 3);
        DEFAULT;
    }
}

void SettingsWindow::window_disappear() {
    Settings::settings()->save();
}
