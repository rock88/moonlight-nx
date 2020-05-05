#include "ContentWindow.hpp"
#pragma once

class MainWindow: public ContentWindow {
public:
    MainWindow(Widget *parent);
    
    void window_appear() override;
    
private:
    void reload();
};
