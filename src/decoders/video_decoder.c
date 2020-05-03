#include "video_decoder.h"
#include "ffmpeg.h"

#define DECODER_BUFFER_SIZE 92 * 1024

static char* ffmpeg_buffer;
AVFrame* frame = NULL;
volatile int frame_is_ready = 0;
pthread_mutex_t mutex;

static int video_decoder_setup(int videoFormat, int width, int height, int redrawRate, void* context, int drFlags) {
    pthread_mutex_init(&mutex, NULL);
    
    printf("decoder_setup: %ix%i\n", width, height);
    
    int avc_flags = SLICE_THREADING;
    
    if (ffmpeg_init(videoFormat, width, height, avc_flags, 2, 2) < 0) {
        fprintf(stderr, "Couldn't initialize video decoding\n");
        return -1;
    }
    
    ffmpeg_buffer = malloc(DECODER_BUFFER_SIZE + AV_INPUT_BUFFER_PADDING_SIZE);
    if (ffmpeg_buffer == NULL) {
        fprintf(stderr, "Not enough memory\n");
        ffmpeg_destroy();
        return -1;
    }
    
    return 0;
}

static void video_decoder_cleanup() {
    ffmpeg_destroy();
    frame = NULL;
}

static int video_decoder_submit_decode_unit(PDECODE_UNIT decodeUnit) {
    if (decodeUnit->fullLength < DECODER_BUFFER_SIZE) {
        PLENTRY entry = decodeUnit->bufferList;
        int length = 0;
        while (entry != NULL) {
            memcpy(ffmpeg_buffer+length, entry->data, entry->length);
            length += entry->length;
            entry = entry->next;
        }
        ffmpeg_decode(ffmpeg_buffer, length);
        
        if (pthread_mutex_lock(&mutex) == 0) {
            frame = ffmpeg_get_frame(false);
            
            // Push event!!
            pthread_mutex_unlock(&mutex);
        }
    }
    return DR_OK;
}

DECODER_RENDERER_CALLBACKS video_decoder_callbacks = {
    .setup = video_decoder_setup,
    .cleanup = video_decoder_cleanup,
    .submitDecodeUnit = video_decoder_submit_decode_unit,
    .capabilities = CAPABILITY_SLICES_PER_FRAME(4) | CAPABILITY_REFERENCE_FRAME_INVALIDATION_AVC | CAPABILITY_REFERENCE_FRAME_INVALIDATION_HEVC | CAPABILITY_DIRECT_SUBMIT,
};
