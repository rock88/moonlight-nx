#include <stdio.h>
#ifdef __cplusplus
#define EXTERN extern "C"
#else
#define EXTERN
#endif

#include <stdint.h>

EXTERN void gl_render_init();
EXTERN void gl_render_setup(int width, int height);
EXTERN void gl_render_draw(uint8_t* image[3]);
