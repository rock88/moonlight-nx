#include "SwitchMoonlightSessionDecoderAndRenderProvider.hpp"
#include "FFmpegVideoDecoder.hpp"
#include "GLVideoRenderer.hpp"
#ifdef __SWITCH__
#include "AudrenAudioRenderer.hpp"
#endif
#include "DebugFileRecorderAudioRenderer.hpp"

IFFmpegVideoDecoder* SwitchMoonlightSessionDecoderAndRenderProvider::video_decoder() {
    return new FFmpegVideoDecoder();
}

IVideoRenderer* SwitchMoonlightSessionDecoderAndRenderProvider::video_renderer() {
    return new GLVideoRenderer();
}

IAudioRenderer* SwitchMoonlightSessionDecoderAndRenderProvider::audio_renderer() {
    #ifdef __SWITCH__
    return new AudrenAudioRenderer();
    #else
    return new DebugFileRecorderAudioRenderer(false);
    #endif
}
