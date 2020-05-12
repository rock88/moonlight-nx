#include "VDPAUVideoDecoder.hpp"
#include "Log.h"
#include <libavcodec/vdpau.h>
#include <libavutil/hwcontext.h>
#include <libavutil/hwcontext_vdpau.h>

// TODO: Currently doesn't work :(
VDPAUVideoDecoder::VDPAUVideoDecoder() {
    putenv("VDPAU_DRIVER=tegra");
    putenv("VDPAU_DRIVER_PATH=/storage/cores");
    
    int err = av_hwdevice_ctx_create(&m_hw_context, AV_HWDEVICE_TYPE_VDPAU, nullptr, nullptr, 0);

    if (err) {
        LOG_FMT("Failed to create VDPAU context: %d\n", err);
    } else {
        LOG("Success create VDPAU context\n");
    }
}

VDPAUVideoDecoder::~VDPAUVideoDecoder() {
    if (m_hw_context) {
        av_buffer_unref(&m_hw_context);
    }
}

bool VDPAUVideoDecoder::prepare_decoder_context(AVCodecContext *context, AVDictionary **options) {
    if (m_hw_context) {
        context->hw_device_ctx = av_buffer_ref(m_hw_context);
        context->hwaccel_flags |= AV_HWACCEL_FLAG_ALLOW_PROFILE_MISMATCH;
        context->hwaccel_flags |= AV_HWACCEL_FLAG_IGNORE_LEVEL;
        
        LOG("Using VDPAU accelerated renderer\n");
        return true;
    }
    
    LOG("Failed to use VDPAU accelerated renderer\n");
    return false;
}
