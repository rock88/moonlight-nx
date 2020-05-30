#include "ContentWindow.hpp"
#include <vector>
#pragma once

class LogsWindow: public ContentWindow {
public:
    LogsWindow(Widget *parent);
    
private:
    void reload();
    
    std::vector<nanogui::Widget *> m_labels;
};
