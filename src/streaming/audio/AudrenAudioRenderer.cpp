#include "AudrenAudioRenderer.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <inttypes.h>
#include "Log.h"

static const AudioRendererConfig m_ar_config =
{
    .output_rate     = AudioRendererOutputRate_48kHz,
    .num_voices      = 24,
    .num_effects     = 0,
    .num_sinks       = 1,
    .num_mix_objs    = 1,
    .num_mix_buffers = 2,
};

AudrenAudioRenderer::AudrenAudioRenderer() {

}

AudrenAudioRenderer::~AudrenAudioRenderer() {
    cleanup();
}

int AudrenAudioRenderer::init(int audio_configuration, const POPUS_MULTISTREAM_CONFIGURATION opus_config, void *context, int ar_flags) {
    m_channel_count = opus_config->channelCount;
    m_samples_datasize = m_channel_count * m_decoded_buffers_before_play * m_samples_per_frame * sizeof(s16);
    
    int mempool_size = (m_samples_datasize + 0xFFF) &~ 0xFFF;
    
    m_decoded_buffer = (s16 *)malloc(m_samples_datasize);
    mempool_ptr = memalign(0x1000, mempool_size);
    
    if (m_decoded_buffer == NULL || mempool_ptr == NULL) {
        LOG_FMT("Failed to allocate buffers\n");
        return -1;
    }
    
    int error;
    m_decoder = opus_multistream_decoder_create(opus_config->sampleRate, opus_config->channelCount, opus_config->streams, opus_config->coupledStreams, opus_config->mapping, &error);
    
    Result res = audrenInitialize(&m_ar_config);
    m_inited_driver = R_SUCCEEDED(res);
    
    if (R_FAILED(res)) {
        LOG_FMT("audrenInitialize failed (0x%x)", res);
    }
    
    res = audrvCreate(&m_driver, &m_ar_config, m_channel_count);
    if (R_FAILED(res)) {
        LOG_FMT("audrvCreate failed (0x%x)", res);
    }
    
    m_wavebuf.data_raw = mempool_ptr;
    m_wavebuf.size = m_samples_datasize;
    m_wavebuf.start_sample_offset = 0;
    m_wavebuf.end_sample_offset = m_decoded_buffers_before_play * m_samples_per_frame;
    
    int mpid = audrvMemPoolAdd(&m_driver, mempool_ptr, mempool_size);
    audrvMemPoolAttach(&m_driver, mpid);
    
    static const u8 sink_channels[] = {0, 1};
    audrvDeviceSinkAdd(&m_driver, AUDREN_DEFAULT_DEVICE_NAME, m_channel_count, sink_channels);
    
    audrvUpdate(&m_driver);
    
    res = audrenStartAudioRenderer();
    if (R_FAILED(res)) {
        LOG_FMT("audrenStartAudioRenderer failed (0x%x)", res);
    }
    
    audrvVoiceInit(&m_driver, 0, m_channel_count, PcmFormat_Int16, opus_config->sampleRate);
    audrvVoiceSetDestinationMix(&m_driver, 0, AUDREN_FINAL_MIX_ID);
    audrvVoiceSetMixFactor(&m_driver, 0, 1.0f, 0, 0);
    audrvVoiceSetMixFactor(&m_driver, 0, 0.0f, 0, 1);
    audrvVoiceSetMixFactor(&m_driver, 0, 0.0f, 1, 0);
    audrvVoiceSetMixFactor(&m_driver, 0, 1.0f, 1, 1);
    audrvVoiceStart(&m_driver, 0);
    
    return DR_OK;
}

void AudrenAudioRenderer::cleanup() {
    if (m_decoder) {
        opus_multistream_decoder_destroy(m_decoder);
        m_decoder = nullptr;
    }
    
    if (m_decoded_buffer) {
        free(m_decoded_buffer);
        m_decoded_buffer = nullptr;
    }
    
    if (mempool_ptr) {
        free(mempool_ptr);
        mempool_ptr = nullptr;
    }
    
    if (m_inited_driver) {
        audrvVoiceStop(&m_driver, 0);
        audrvClose(&m_driver);
        audrenExit();
        m_inited_driver = false;
    }
}

void AudrenAudioRenderer::decode_and_play_sample(char *data, int length) {
    s16* ptr = &m_decoded_buffer[m_current_frame * m_samples_per_frame * m_channel_count];
    int decoded_samples = opus_multistream_decode(m_decoder, (const unsigned char *)data, length, ptr, m_samples_per_frame, 0);
    
    if (decoded_samples > 0) {
        m_current_frame++;
        m_total_decoded_samples += decoded_samples;
        
        if (m_current_frame < m_decoded_buffers_before_play) {
            return;
        }
        
        LOG_FMT("audrvVoiceIsPlaying: %i\n", audrvVoiceIsPlaying(&m_driver, 0));
        
        audrvVoiceStop(&m_driver, 0);
        
        memcpy(mempool_ptr, m_decoded_buffer, m_samples_datasize);
        armDCacheFlush(mempool_ptr, (m_samples_datasize + 0xFFF) &~ 0xFFF);

        m_wavebuf.size = m_total_decoded_samples * m_channel_count * sizeof(s16);
        m_wavebuf.end_sample_offset = m_total_decoded_samples;

        audrvVoiceAddWaveBuf(&m_driver, 0, &m_wavebuf);
        audrvVoiceStart(&m_driver, 0);

        m_current_frame = 0;
        m_total_decoded_samples = 0;

        Result res = audrvUpdate(&m_driver);
        if (R_FAILED(res)) {
            LOG_FMT("audrvUpdate: %" PRIx32 "\n", res);
        }
    }
}

int AudrenAudioRenderer::capabilities() {
    return CAPABILITY_DIRECT_SUBMIT;
}
