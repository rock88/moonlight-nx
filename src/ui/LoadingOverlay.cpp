#include "LoadingOverlay.hpp"
#include "nanovg.h"

using namespace nanogui;

LoadingOverlay::LoadingOverlay(Widget* parent): Widget(parent->screen()) {
    set_fixed_size(parent->screen()->size());
    
    m_icon = FA_SPINNER;
    screen()->perform_layout();
}

void LoadingOverlay::draw(NVGcontext *ctx) {
    nvgSave(ctx);
    
    // Draw bg
    nvgFillColor(ctx, Color(0, 0, 0, 100));
    nvgBeginPath(ctx);
    nvgRect(ctx, 0, 0, width(), height());
    nvgFill(ctx);
    
    // Draw spinner
    static double r = 0;
    r += 0.05;
    
    nvgTranslate(ctx, width() / 2, height() / 2);
    nvgRotate(ctx, r);
    
    nvgFillColor(ctx, Color(255, 255, 255, 255));
    nvgFontSize(ctx, 40);
    nvgFontFace(ctx, "icons");
    nvgTextAlign(ctx, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
    nvgText(ctx, 0, 0, utf8(m_icon).data(), NULL);
    
    nvgRestore(ctx);
}

bool LoadingOverlay::mouse_enter_event(const nanogui::Vector2i &p, bool enter) {
    return true;
}

bool LoadingOverlay::mouse_button_event(const nanogui::Vector2i &p, int button, bool down, int modifiers) {
    return true;
}

bool LoadingOverlay::scroll_event(const nanogui::Vector2i &p, const nanogui::Vector2f &rel) {
    return true;
}

void LoadingOverlay::dispose() {
    screen()->remove_child(this);
}
