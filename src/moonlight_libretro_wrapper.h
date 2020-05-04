#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
#define EXTERN extern "C"
#else
#define EXTERN extern
#endif

EXTERN int16_t keyboard_state[];

struct GamePadState {
    short buttonFlags;
    unsigned char leftTrigger;
    unsigned char rightTrigger;
    short leftStickX;
    short leftStickY;
    short rightStickX;
    short rightStickY;
};

EXTERN struct GamePadState game_pad_state;

EXTERN void moonlight_libretro_wrapper_preinit();
EXTERN void moonlight_libretro_wrapper_init(int width, int height);
EXTERN void moonlight_libretro_wrapper_set_working_dir(const char* dir);
EXTERN void moonlight_libretro_wrapper_handle_mouse_move(double x, double y);
EXTERN void moonlight_libretro_wrapper_handle_mouse_button(int button, int action, int modifiers);
EXTERN void moonlight_libretro_wrapper_handle_game_pad();
EXTERN void moonlight_libretro_wrapper_draw();
