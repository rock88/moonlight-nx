#include "ContentWindow.hpp"
#pragma once

class LogsWindow: public ContentWindow {
public:
    LogsWindow(Widget *parent);
    
private:
    void reload();
};
