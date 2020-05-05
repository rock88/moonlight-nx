#include "ContentWindow.hpp"

class SettingsWindow: public ContentWindow {
public:
    SettingsWindow(nanogui::Widget* parent);
    
    void window_disappear() override;
};
