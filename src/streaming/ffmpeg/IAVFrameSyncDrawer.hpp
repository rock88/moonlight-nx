#include <functional>
#pragma once

struct AVFrame;

using SyncDrawFunc = std::function<void(std::function<void(AVFrame* frame)>)>;

class IAVFrameSyncDrawer {
public:
    virtual void sync_draw(SyncDrawFunc func) = 0;
};
