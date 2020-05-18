#include "ContentWindow.hpp"
#pragma once

class MainWindow: public ContentWindow {
public:
    MainWindow(Widget *parent);
    
    void window_appear() override;
    
    void draw(NVGcontext *ctx) override;
    
    bool keyboard_event(int key, int scancode, int action, int modifiers) override;
    
private:
    void reload();
};
