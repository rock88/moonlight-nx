#include "LogsWindow.hpp"
#include "Settings.hpp"
#include "Data.hpp"

using namespace nanogui;

LogsWindow::LogsWindow(Widget *parent): ContentWindow(parent, "Logs") {
    set_left_pop_button();
    set_right_title_button(FA_TRASH_ALT, [this] {
        remove(Settings::settings()->log_path().c_str());
        this->reload();
    });
    set_box_layout(Orientation::Vertical, Alignment::Minimum);
    
    reload();
}

void LogsWindow::reload() {
    for (auto label: m_labels) {
        if (container()->child_index(label) != -1) {
            container()->remove_child(label);
        }
    }
    m_labels.clear();
    
    Data data = Data::read_from_file(Settings::settings()->log_path());
    
    if (data.is_empty()) {
        auto label = container()->add<Label>("No logs...");
        m_labels.push_back(label);
    } else {
        std::stringstream stream((char *)data.bytes());
        std::string string;
        
        while (std::getline(stream, string, '\n')) {
            auto label = container()->add<Label>(string);
            label->set_selectable(true);
            m_labels.push_back(label);
        }
    }
    perform_layout();
}
