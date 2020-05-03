#include "audio_decoder.h"
#include "libretro.h"
#include <opus/opus_multistream.h>

#define MAX_CHANNEL_COUNT 6
#define FRAME_SIZE 240
#define FRAME_BUFFER 12

static OpusMSDecoder* decoder;
static short pcmBuffer[FRAME_SIZE * MAX_CHANNEL_COUNT];
static int channelCount;

static int audio_decoder_init(int audioConfiguration, POPUS_MULTISTREAM_CONFIGURATION opusConfig, void* context, int arFlags) {
    int error;
    decoder = opus_multistream_decoder_create(opusConfig->sampleRate, opusConfig->channelCount, opusConfig->streams, opusConfig->coupledStreams, opusConfig->mapping, &error);
    channelCount = opusConfig->channelCount;
    return 0;
}

extern retro_audio_sample_batch_t audio_batch_cb;

static void audio_decoder_decode_and_play_sample(char* data, int length) {
    if (audio_batch_cb) {
        int decodeLen = opus_multistream_decode(decoder, data, length, pcmBuffer, FRAME_SIZE, 0);
        if (decodeLen > 0) {
            audio_batch_cb(pcmBuffer, decodeLen);
        }
    }
}

static void audio_decoder_cleanup() {
    if (decoder != NULL) {
        opus_multistream_decoder_destroy(decoder);
    }
}

AUDIO_RENDERER_CALLBACKS audio_decoder_callbacks = {
    .init = audio_decoder_init,
    .cleanup = audio_decoder_cleanup,
    .decodeAndPlaySample = audio_decoder_decode_and_play_sample,
    .capabilities = CAPABILITY_DIRECT_SUBMIT,
};
