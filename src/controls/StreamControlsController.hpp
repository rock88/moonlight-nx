#include "Singleton.hpp"
#include "MouseController.hpp"
#include "KeyboardController.hpp"
#pragma once

class StreamControlsController: public Singleton<StreamControlsController> {
public:
    void send_to_stream(int width, int height);

private:
    MouseState m_mouse_state = {0};
    KeyboardState m_keyboard_state;
};
