#include <stdio.h>
#include <stdint.h>
#include <libavcodec/avcodec.h>

#ifdef __cplusplus
#define EXTERN extern "C"
#else
#define EXTERN
#endif

EXTERN void gl_render_init();
EXTERN void gl_render_setup(int width, int height);
EXTERN void gl_render_draw(uint8_t* image[3], enum AVColorSpace color_space, enum AVColorRange color_range);
