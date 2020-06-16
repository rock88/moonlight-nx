#include "ContentWindow.hpp"
#pragma once

struct Host;

class MainWindow: public ContentWindow {
public:
    MainWindow(Widget *parent);
    
    void window_appear() override;
    
    void draw(NVGcontext *ctx) override;
    
private:
    void reload();
    void wake_up_host(const Host &host);
};
