#include "Limelight.h"
#include <libavcodec/avcodec.h>
#include <pthread.h>

extern volatile int frame_is_ready;
extern pthread_mutex_t mutex;
extern AVFrame* frame;
extern DECODER_RENDERER_CALLBACKS video_decoder_callbacks;
