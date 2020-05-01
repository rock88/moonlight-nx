#include "Application.hpp"
#include "MainWindow.hpp"

using namespace nanogui;

Application::Application(Vector2f size, Vector2f framebuffer_size): Screen(size, framebuffer_size) {
    push_window<MainWindow>();
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
    
}
