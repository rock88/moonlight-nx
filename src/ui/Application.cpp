#include "Application.hpp"
#include "MainWindow.hpp"
#include "AddHostWindow.hpp"

using namespace nanogui;

Application::Application(Vector2f size, Vector2f framebuffer_size): Screen(size, framebuffer_size) {
    auto main = push_window<MainWindow>();
    main->set_add_host_callback([this] {
        this->push_window<AddHostWindow>();
    });
}

template<typename WidgetClass, typename... Args>
WidgetClass* Application::push_window(const Args&... args) {
    for (auto window: m_windows) {
        window->set_visible(false);
    }
    
    auto window = new WidgetClass(this, args...);
    window->set_size(size());
    window->set_fixed_size(size());
    m_windows.push_back(window);
    perform_layout();
    return window;
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
}
