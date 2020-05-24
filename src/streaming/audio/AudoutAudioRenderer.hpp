#include "IAudioRenderer.hpp"
#include <opus/opus_multistream.h>
#include <switch.h>
#pragma once

class AudoutAudioRenderer: public IAudioRenderer {
public:
    AudoutAudioRenderer(int audio_delay);
    ~AudoutAudioRenderer();
    
    int init(int audio_configuration, const POPUS_MULTISTREAM_CONFIGURATION opus_config, void *context, int ar_flags) override;
    void cleanup() override;
    void decode_and_play_sample(char *sample_data, int sample_length) override;
    int capabilities() override;
    
private:
    OpusMSDecoder* m_decoder = nullptr;
    s16* m_decoded_buffer = nullptr;
    void* mempool_ptr = nullptr;
    
    AudioOutBuffer audout_buf;
    AudioOutBuffer *audout_released_buf;
    
    int m_channel_count = 0;
    int m_samples_per_frame = AUDREN_SAMPLES_PER_FRAME_48KHZ;
    int m_audio_delay;
    int m_mempool_size = 0, m_samples_datasize = 0;
    
    int m_current_frame = 0;
    s32 m_total_decoded_samples = 0;
};
