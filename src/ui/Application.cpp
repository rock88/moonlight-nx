#include "Application.hpp"
#include "MainWindow.hpp"
#include "AddHostWindow.hpp"
#include "StreamWindow.hpp"

using namespace nanogui;

Application::Application(Vector2f size, Vector2f framebuffer_size): Screen(size, framebuffer_size) {
    theme()->m_button_font_size = 24;
    theme()->m_text_box_font_size = 34;
    
    push_window<MainWindow>();
}

void Application::pop_window() {
    if (m_windows.size() > 1) {
        auto last = m_windows.back();
        m_windows.pop_back();
        update_focus(NULL);
        remove_child(last);
        m_windows.back()->set_visible(true);
        perform_layout();
    }
    
    if (m_windows.size() == 1) {
        auto main = static_cast<MainWindow *>(m_windows.front());
        main->reload();
    }
}
