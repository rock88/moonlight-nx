#include "RetroAudioRenderer.hpp"
#include "libretro.h"
#include <cstdlib>

#define MAX_CHANNEL_COUNT 6
#define FRAME_SIZE 240

extern retro_audio_sample_batch_t audio_batch_cb;

RetroAudioRenderer::~RetroAudioRenderer() {
    cleanup();
}

int RetroAudioRenderer::init(int audio_configuration, const POPUS_MULTISTREAM_CONFIGURATION opus_config, void *context, int ar_flags) {
    int error;
    m_decoder = opus_multistream_decoder_create(opus_config->sampleRate, opus_config->channelCount, opus_config->streams, opus_config->coupledStreams, opus_config->mapping, &error);
    m_buffer = (short *)malloc(FRAME_SIZE * MAX_CHANNEL_COUNT * sizeof(short));
    return DR_OK;
}

void RetroAudioRenderer::cleanup() {
    if (m_decoder) {
        opus_multistream_decoder_destroy(m_decoder);
        m_decoder = nullptr;
    }
    
    if (m_buffer) {
        free(m_buffer);
        m_buffer = nullptr;
    }
}

void RetroAudioRenderer::decode_and_play_sample(char *data, int length) {
    if (audio_batch_cb) {
        int decode_len = opus_multistream_decode(m_decoder, (const unsigned char *)data, length, m_buffer, FRAME_SIZE, 0);
        if (decode_len > 0) {
            audio_batch_cb(m_buffer, decode_len);
        }
    }
}

int RetroAudioRenderer::capabilities() {
    return CAPABILITY_DIRECT_SUBMIT;
}
