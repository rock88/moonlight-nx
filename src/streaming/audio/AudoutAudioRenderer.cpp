#include "AudoutAudioRenderer.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <inttypes.h>
#include "Log.h"

AudoutAudioRenderer::AudoutAudioRenderer() {

}

AudoutAudioRenderer::~AudoutAudioRenderer() {
    cleanup();
}

int AudoutAudioRenderer::init(int audio_configuration, const POPUS_MULTISTREAM_CONFIGURATION opus_config, void *context, int ar_flags) {
    m_channel_count = opus_config->channelCount;
    m_samples_datasize = m_channel_count * m_decoded_buffers_before_play * m_samples_per_frame * sizeof(s16);
    m_mempool_size = (m_samples_datasize + 0xFFF) &~ 0xFFF;
    
    m_decoded_buffer = (s16 *)malloc(m_samples_datasize);
    mempool_ptr = memalign(0x1000, m_mempool_size);
    
    if (m_decoded_buffer == NULL || mempool_ptr == NULL) {
        LOG("Failed to allocate buffers\n");
        return -1;
    }
    
    Result rc = audoutInitialize();
    LOG_FMT("audoutInitialize() returned 0x%x\n", rc);
    
    int error;
    m_decoder = opus_multistream_decoder_create(opus_config->sampleRate, opus_config->channelCount, opus_config->streams, opus_config->coupledStreams, opus_config->mapping, &error);
    
    return DR_OK;
}

void AudoutAudioRenderer::cleanup() {
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
    
    audoutStopAudioOut();
    audoutExit();
}

void AudoutAudioRenderer::decode_and_play_sample(char *data, int length) {
    s16* ptr = &m_decoded_buffer[m_current_frame * m_samples_per_frame * m_channel_count];
    int decoded_samples = opus_multistream_decode(m_decoder, (const unsigned char *)data, length, ptr, m_samples_per_frame, 0);
    
    if (decoded_samples > 0) {
        m_current_frame++;
        m_total_decoded_samples += decoded_samples;
        
        if (m_current_frame < m_decoded_buffers_before_play) {
            return;
        }
        
        memcpy(mempool_ptr, m_decoded_buffer, m_samples_datasize);
        armDCacheFlush(mempool_ptr, (m_samples_datasize + 0xFFF) &~ 0xFFF);
        
        audoutStopAudioOut();
        
        // Prepare the audio data source buffer.
        audout_buf.next = NULL;
        audout_buf.buffer = mempool_ptr;
        audout_buf.buffer_size = m_mempool_size;
        audout_buf.data_size = m_samples_datasize;
        audout_buf.data_offset = 0;
        
        // Prepare pointer for the released buffer.
        audout_released_buf = NULL;
        
        // Play the buffer.
        audoutStartAudioOut();
        Result rc = audoutPlayBuffer(&audout_buf, &audout_released_buf);
        
        if (R_FAILED(rc))
            LOG_FMT("audoutPlayBuffer() returned 0x%x\n", rc);
        
        m_current_frame = 0;
        m_total_decoded_samples = 0;
    }
}

int AudoutAudioRenderer::capabilities() {
    return CAPABILITY_DIRECT_SUBMIT;
}
