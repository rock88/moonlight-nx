#include "AppButton.hpp"
#include "Application.hpp"
#include "GameStreamClient.hpp"
#include "nanovg.h"

using namespace nanogui;

#define BLANK_IMAGE_SIZE 6129

static std::map<int, std::pair<char*, size_t>> m_image_data;
static std::map<int, int> m_image_handle;
static std::vector<int> m_blank_images;

AppButton::AppButton(Widget* parent, const std::string &address, APP_LIST app, int currentGame): Button(parent, "") {
    m_address = address;
    m_app = app;
    
    set_fixed_size(Size(210, 296));
    set_layout(new BoxLayout(Orientation::Vertical, Alignment::Middle));
    
    m_label = add<Label>(m_app.name);
    m_label->set_fixed_width(fixed_width());
    
    if (m_image_handle.count(m_app.id) && std::find(m_blank_images.begin(), m_blank_images.end(), m_image_handle[m_app.id]) == m_blank_images.end()) {
        m_label->set_visible(false);
    }
    
    if (m_app.id == currentGame) {
        m_label->set_caption(m_label->caption() + " (Running)");
    }
    
    if (!m_image_handle.count(m_app.id) || !m_image_data.count(m_app.id)) {
        inc_ref();
        GameStreamClient::client()->app_boxart(m_address, m_app.id, [this](auto result) {
            if (result.isSuccess()) {
                m_label->set_visible(result.value().second == BLANK_IMAGE_SIZE);
                m_image_data[m_app.id] = result.value();
            } else {
                m_image_handle[m_app.id] = -1;
            }
            
            dec_ref();
        });
    }
}

void AppButton::draw(NVGcontext *ctx) {
    if (m_image_handle.count(m_app.id) && m_image_handle[m_app.id] != -1) {
        nvgSave(ctx);
        
        int img = m_image_handle[m_app.id];
        int w, h;
        nvgImageSize(ctx, img, &w, &h);
        
        nvgBeginPath(ctx);
        nvgTranslate(ctx, m_pos.x(), m_pos.y());
        NVGpaint paint = nvgImagePattern(ctx, 0, 0, fixed_width(), fixed_height(), 0, img, 1);
        nvgRect(ctx, 0, 0, fixed_width(), fixed_height());
        nvgFillPaint(ctx, paint);
        nvgFill(ctx);
        nvgRestore(ctx);
    } else if (m_image_data.count(m_app.id)) {
        // Upload image, draw at next iteration...
        int img = nvgCreateImageMem(ctx, 0, (unsigned char*)m_image_data[m_app.id].first, (int)m_image_data[m_app.id].second);
        m_image_handle[m_app.id] = img;
        
        if (m_image_data[m_app.id].second == BLANK_IMAGE_SIZE) {
            m_blank_images.push_back(img);
        }
        m_image_data.erase(m_app.id);
    }
    
    Button::draw(ctx);
    
    if (m_label->visible()) {
        nvgSave(ctx);
        nvgFillColor(ctx, Color(0, 0, 0, 100));
        nvgBeginPath(ctx);
        nvgRect(ctx, m_pos.x(), m_pos.y(), m_label->width(), m_label->height());
        nvgFill(ctx);
        nvgRestore(ctx);
        
        Widget::draw(ctx);
    }
}
