#include <Limelight.h>
#pragma once

extern "C" {
    #include <libavcodec/avcodec.h>
}

class IVideoRenderer {
public:
    virtual ~IVideoRenderer() {};
    virtual void draw(int width, int height, AVFrame* frame) = 0;
};
