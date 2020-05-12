#include "FFmpegVideoDecoder.hpp"
#pragma once

class VDPAUVideoDecoder: public IFFmpegHardwareVideoDecoder {
public:
    VDPAUVideoDecoder();
    ~VDPAUVideoDecoder();
    
    bool prepare_decoder_context(AVCodecContext *context, AVDictionary **options) override;
    
private:
    AVBufferRef* m_hw_context = nullptr;
};
