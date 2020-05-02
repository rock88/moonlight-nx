#include <stdio.h>
#ifdef __cplusplus
#define EXTERN extern "C"
#else
#define EXTERN
#endif

EXTERN void gl_render_init();
EXTERN void gl_render_setup(int width, int height);
EXTERN void gl_render_draw(char* image[3]);
