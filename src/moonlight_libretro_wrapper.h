#include <stdio.h>

#ifdef __cplusplus
#define EXTERN extern "C"
#else
#define EXTERN extern
#endif

EXTERN int16_t keyboard_state[];

EXTERN void moonlight_libretro_wrapper_preinit();
EXTERN void moonlight_libretro_wrapper_init(int width, int height);
EXTERN void moonlight_libretro_wrapper_set_working_dir(const char* dir);
EXTERN void moonlight_libretro_wrapper_handle_mouse_move(double x, double y);
EXTERN void moonlight_libretro_wrapper_handle_mouse_button(int button, int action, int modifiers);
EXTERN void moonlight_libretro_wrapper_draw();
