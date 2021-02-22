// Some mock part of libnx stuff for import and use <switch.h> on others plaforms
#include <stdlib.h>

#pragma once

typedef uint8_t u8;       ///<   8-bit unsigned integer.
typedef uint16_t u16;     ///<  16-bit unsigned integer.
typedef uint32_t u32;     ///<  32-bit unsigned integer.
typedef uint64_t u64;     ///<  64-bit unsigned integer.
typedef __uint128_t u128; ///< 128-bit unsigned integer.

typedef int8_t s8;       ///<   8-bit signed integer.
typedef int16_t s16;     ///<  16-bit signed integer.
typedef int32_t s32;     ///<  32-bit signed integer.
typedef int64_t s64;     ///<  64-bit signed integer.
typedef __int128_t s128; ///< 128-bit unsigned integer.

typedef volatile u8 vu8;     ///<   8-bit volatile unsigned integer.
typedef volatile u16 vu16;   ///<  16-bit volatile unsigned integer.
typedef volatile u32 vu32;   ///<  32-bit volatile unsigned integer.
typedef volatile u64 vu64;   ///<  64-bit volatile unsigned integer.
typedef volatile u128 vu128; ///< 128-bit volatile unsigned integer.

typedef volatile s8 vs8;     ///<   8-bit volatile signed integer.
typedef volatile s16 vs16;   ///<  16-bit volatile signed integer.
typedef volatile s32 vs32;   ///<  32-bit volatile signed integer.
typedef volatile s64 vs64;   ///<  64-bit volatile signed integer.
typedef volatile s128 vs128; ///< 128-bit volatile signed integer.

typedef u32 Handle;                 ///< Kernel object handle.
typedef u32 Result;                 ///< Function error code result type.

/// Checks whether a result code indicates success.
#define R_SUCCEEDED(res)   ((res)==0)
/// Checks whether a result code indicates failure.
#define R_FAILED(res)      ((res)!=0)

#define BIT(n) (1U<<(n))

typedef struct HidMouseState {
    u64 sampling_number;                        ///< SamplingNumber
    s32 x;                                      ///< X
    s32 y;                                      ///< Y
    s32 delta_x;                                ///< DeltaX
    s32 delta_y;                                ///< DeltaY
    s32 wheel_delta_x;                          ///< WheelDeltaX
    s32 wheel_delta_y;                          ///< WheelDeltaY
    u32 buttons;                                ///< Bitfield of \ref HidMouseButton.
    u32 attributes;                             ///< Bitfield of \ref HidMouseAttribute.
} HidMouseState;

typedef enum {
    HidMouseAttribute_Transferable     = BIT(0),    ///< Transferable
    HidMouseAttribute_IsConnected      = BIT(1),    ///< IsConnected
} HidMouseAttribute;

typedef enum {
    HidMouseButton_Left    = BIT(0),
    HidMouseButton_Right   = BIT(1),
    HidMouseButton_Middle  = BIT(2),
    HidMouseButton_Forward = BIT(3),
    HidMouseButton_Back    = BIT(4),
} HidMouseButton;

inline static void hidInitializeMouse(void) {}

inline static size_t hidGetMouseStates(HidMouseState *states, size_t count) {
    return 0;
}

inline static void hidInitializeTouchScreen(void) {}

/// HidTouchState
typedef struct HidTouchState {
    u64 delta_time;                             ///< DeltaTime
    u32 attributes;                             ///< Bitfield of \ref HidTouchAttribute.
    u32 finger_id;                              ///< FingerId
    u32 x;                                      ///< X
    u32 y;                                      ///< Y
    u32 diameter_x;                             ///< DiameterX
    u32 diameter_y;                             ///< DiameterY
    u32 rotation_angle;                         ///< RotationAngle
    u32 reserved;                               ///< Reserved
} HidTouchState;

/// HidTouchScreenState
typedef struct HidTouchScreenState {
    u64 sampling_number;                        ///< SamplingNumber
    s32 count;                                  ///< Number of entries in the touches array.
    u32 reserved;                               ///< Reserved
    HidTouchState touches[16];                  ///< Array of \ref HidTouchState, with the above count.
} HidTouchScreenState;

inline static size_t hidGetTouchScreenStates(HidTouchScreenState *states, size_t count) {
    return 0;
}

/// HidKeyboardScancode deprecated
typedef enum DEPRECATED {
    KBD_NONE = 0x00,
    KBD_ERR_OVF = 0x01,

    KBD_A = 0x04,
    KBD_B = 0x05,
    KBD_C = 0x06,
    KBD_D = 0x07,
    KBD_E = 0x08,
    KBD_F = 0x09,
    KBD_G = 0x0a,
    KBD_H = 0x0b,
    KBD_I = 0x0c,
    KBD_J = 0x0d,
    KBD_K = 0x0e,
    KBD_L = 0x0f,
    KBD_M = 0x10,
    KBD_N = 0x11,
    KBD_O = 0x12,
    KBD_P = 0x13,
    KBD_Q = 0x14,
    KBD_R = 0x15,
    KBD_S = 0x16,
    KBD_T = 0x17,
    KBD_U = 0x18,
    KBD_V = 0x19,
    KBD_W = 0x1a,
    KBD_X = 0x1b,
    KBD_Y = 0x1c,
    KBD_Z = 0x1d,

    KBD_1 = 0x1e,
    KBD_2 = 0x1f,
    KBD_3 = 0x20,
    KBD_4 = 0x21,
    KBD_5 = 0x22,
    KBD_6 = 0x23,
    KBD_7 = 0x24,
    KBD_8 = 0x25,
    KBD_9 = 0x26,
    KBD_0 = 0x27,

    KBD_ENTER = 0x28,
    KBD_ESC = 0x29,
    KBD_BACKSPACE = 0x2a,
    KBD_TAB = 0x2b,
    KBD_SPACE = 0x2c,
    KBD_MINUS = 0x2d,
    KBD_EQUAL = 0x2e,
    KBD_LEFTBRACE = 0x2f,
    KBD_RIGHTBRACE = 0x30,
    KBD_BACKSLASH = 0x31,
    KBD_HASHTILDE = 0x32,
    KBD_SEMICOLON = 0x33,
    KBD_APOSTROPHE = 0x34,
    KBD_GRAVE = 0x35,
    KBD_COMMA = 0x36,
    KBD_DOT = 0x37,
    KBD_SLASH = 0x38,
    KBD_CAPSLOCK = 0x39,

    KBD_F1 = 0x3a,
    KBD_F2 = 0x3b,
    KBD_F3 = 0x3c,
    KBD_F4 = 0x3d,
    KBD_F5 = 0x3e,
    KBD_F6 = 0x3f,
    KBD_F7 = 0x40,
    KBD_F8 = 0x41,
    KBD_F9 = 0x42,
    KBD_F10 = 0x43,
    KBD_F11 = 0x44,
    KBD_F12 = 0x45,

    KBD_SYSRQ = 0x46,
    KBD_SCROLLLOCK = 0x47,
    KBD_PAUSE = 0x48,
    KBD_INSERT = 0x49,
    KBD_HOME = 0x4a,
    KBD_PAGEUP = 0x4b,
    KBD_DELETE = 0x4c,
    KBD_END = 0x4d,
    KBD_PAGEDOWN = 0x4e,
    KBD_RIGHT = 0x4f,
    KBD_LEFT = 0x50,
    KBD_DOWN = 0x51,
    KBD_UP = 0x52,

    KBD_NUMLOCK = 0x53,
    KBD_KPSLASH = 0x54,
    KBD_KPASTERISK = 0x55,
    KBD_KPMINUS = 0x56,
    KBD_KPPLUS = 0x57,
    KBD_KPENTER = 0x58,
    KBD_KP1 = 0x59,
    KBD_KP2 = 0x5a,
    KBD_KP3 = 0x5b,
    KBD_KP4 = 0x5c,
    KBD_KP5 = 0x5d,
    KBD_KP6 = 0x5e,
    KBD_KP7 = 0x5f,
    KBD_KP8 = 0x60,
    KBD_KP9 = 0x61,
    KBD_KP0 = 0x62,
    KBD_KPDOT = 0x63,

    KBD_102ND = 0x64,
    KBD_COMPOSE = 0x65,
    KBD_POWER = 0x66,
    KBD_KPEQUAL = 0x67,

    KBD_F13 = 0x68,
    KBD_F14 = 0x69,
    KBD_F15 = 0x6a,
    KBD_F16 = 0x6b,
    KBD_F17 = 0x6c,
    KBD_F18 = 0x6d,
    KBD_F19 = 0x6e,
    KBD_F20 = 0x6f,
    KBD_F21 = 0x70,
    KBD_F22 = 0x71,
    KBD_F23 = 0x72,
    KBD_F24 = 0x73,

    KBD_OPEN = 0x74,
    KBD_HELP = 0x75,
    KBD_PROPS = 0x76,
    KBD_FRONT = 0x77,
    KBD_STOP = 0x78,
    KBD_AGAIN = 0x79,
    KBD_UNDO = 0x7a,
    KBD_CUT = 0x7b,
    KBD_COPY = 0x7c,
    KBD_PASTE = 0x7d,
    KBD_FIND = 0x7e,
    KBD_MUTE = 0x7f,
    KBD_VOLUMEUP = 0x80,
    KBD_VOLUMEDOWN = 0x81,
    KBD_CAPSLOCK_ACTIVE = 0x82 ,
    KBD_NUMLOCK_ACTIVE = 0x83 ,
    KBD_SCROLLLOCK_ACTIVE = 0x84 ,
    KBD_KPCOMMA = 0x85,

    KBD_KPLEFTPAREN = 0xb6,
    KBD_KPRIGHTPAREN = 0xb7,

    KBD_LEFTCTRL = 0xe0,
    KBD_LEFTSHIFT = 0xe1,
    KBD_LEFTALT = 0xe2,
    KBD_LEFTMETA = 0xe3,
    KBD_RIGHTCTRL = 0xe4,
    KBD_RIGHTSHIFT = 0xe5,
    KBD_RIGHTALT = 0xe6,
    KBD_RIGHTMETA = 0xe7,

    KBD_MEDIA_PLAYPAUSE = 0xe8,
    KBD_MEDIA_STOPCD = 0xe9,
    KBD_MEDIA_PREVIOUSSONG = 0xea,
    KBD_MEDIA_NEXTSONG = 0xeb,
    KBD_MEDIA_EJECTCD = 0xec,
    KBD_MEDIA_VOLUMEUP = 0xed,
    KBD_MEDIA_VOLUMEDOWN = 0xee,
    KBD_MEDIA_MUTE = 0xef,
    KBD_MEDIA_WWW = 0xf0,
    KBD_MEDIA_BACK = 0xf1,
    KBD_MEDIA_FORWARD = 0xf2,
    KBD_MEDIA_STOP = 0xf3,
    KBD_MEDIA_FIND = 0xf4,
    KBD_MEDIA_SCROLLUP = 0xf5,
    KBD_MEDIA_SCROLLDOWN = 0xf6,
    KBD_MEDIA_EDIT = 0xf7,
    KBD_MEDIA_SLEEP = 0xf8,
    KBD_MEDIA_COFFEE = 0xf9,
    KBD_MEDIA_REFRESH = 0xfa,
    KBD_MEDIA_CALC = 0xfb
} HidKeyboardScancode;

/// HidKeyboardModifier
typedef enum {
    HidKeyboardModifier_Control       = BIT(0),
    HidKeyboardModifier_Shift         = BIT(1),
    HidKeyboardModifier_LeftAlt       = BIT(2),
    HidKeyboardModifier_RightAlt      = BIT(3),
    HidKeyboardModifier_Gui           = BIT(4),
    HidKeyboardModifier_CapsLock      = BIT(8),
    HidKeyboardModifier_ScrollLock    = BIT(9),
    HidKeyboardModifier_NumLock       = BIT(10),
    HidKeyboardModifier_Katakana      = BIT(11),
    HidKeyboardModifier_Hiragana      = BIT(12),
} HidKeyboardModifier;

/// HidKeyboardState
typedef struct HidKeyboardState {
    u64 sampling_number;                        ///< SamplingNumber
    u64 modifiers;                              ///< Bitfield of \ref HidKeyboardModifier.
    u64 keys[4];
} HidKeyboardState;

inline static void hidInitializeKeyboard(void) {}

inline static size_t hidGetKeyboardStates(HidKeyboardState *states, size_t count) {
    return 0;
}

/// HidNpadButton
typedef enum {
    HidNpadButton_A             = BIT(0),  ///< A button / Right face button
    HidNpadButton_B             = BIT(1),  ///< B button / Down face button
    HidNpadButton_X             = BIT(2),  ///< X button / Up face button
    HidNpadButton_Y             = BIT(3),  ///< Y button / Left face button
    HidNpadButton_StickL        = BIT(4),  ///< Left Stick button
    HidNpadButton_StickR        = BIT(5),  ///< Right Stick button
    HidNpadButton_L             = BIT(6),  ///< L button
    HidNpadButton_R             = BIT(7),  ///< R button
    HidNpadButton_ZL            = BIT(8),  ///< ZL button
    HidNpadButton_ZR            = BIT(9),  ///< ZR button
    HidNpadButton_Plus          = BIT(10), ///< Plus button
    HidNpadButton_Minus         = BIT(11), ///< Minus button
    HidNpadButton_Left          = BIT(12), ///< D-Pad Left button
    HidNpadButton_Up            = BIT(13), ///< D-Pad Up button
    HidNpadButton_Right         = BIT(14), ///< D-Pad Right button
    HidNpadButton_Down          = BIT(15), ///< D-Pad Down button
    HidNpadButton_StickLLeft    = BIT(16), ///< Left Stick pseudo-button when moved Left
    HidNpadButton_StickLUp      = BIT(17), ///< Left Stick pseudo-button when moved Up
    HidNpadButton_StickLRight   = BIT(18), ///< Left Stick pseudo-button when moved Right
    HidNpadButton_StickLDown    = BIT(19), ///< Left Stick pseudo-button when moved Down
    HidNpadButton_StickRLeft    = BIT(20), ///< Right Stick pseudo-button when moved Left
    HidNpadButton_StickRUp      = BIT(21), ///< Right Stick pseudo-button when moved Up
    HidNpadButton_StickRRight   = BIT(22), ///< Right Stick pseudo-button when moved Right
    HidNpadButton_StickRDown    = BIT(23), ///< Right Stick pseudo-button when moved Left
    HidNpadButton_LeftSL        = BIT(24), ///< SL button on Left Joy-Con
    HidNpadButton_LeftSR        = BIT(25), ///< SR button on Left Joy-Con
    HidNpadButton_RightSL       = BIT(26), ///< SL button on Right Joy-Con
    HidNpadButton_RightSR       = BIT(27), ///< SR button on Right Joy-Con
    HidNpadButton_Palma         = BIT(28), ///< Top button on Poké Ball Plus (Palma) controller
    HidNpadButton_29            = BIT(29),
    HidNpadButton_HandheldLeftB = BIT(30), ///< B button on Left NES/HVC controller in Handheld mode

    HidNpadButton_AnyLeft  = HidNpadButton_Left   | HidNpadButton_StickLLeft  | HidNpadButton_StickRLeft,  ///< Bitmask containing all buttons that are considered Left (D-Pad, Sticks)
    HidNpadButton_AnyUp    = HidNpadButton_Up     | HidNpadButton_StickLUp    | HidNpadButton_StickRUp,    ///< Bitmask containing all buttons that are considered Up (D-Pad, Sticks)
    HidNpadButton_AnyRight = HidNpadButton_Right  | HidNpadButton_StickLRight | HidNpadButton_StickRRight, ///< Bitmask containing all buttons that are considered Right (D-Pad, Sticks)
    HidNpadButton_AnyDown  = HidNpadButton_Down   | HidNpadButton_StickLDown  | HidNpadButton_StickRDown,  ///< Bitmask containing all buttons that are considered Down (D-Pad, Sticks)
    HidNpadButton_AnySL    = HidNpadButton_LeftSL | HidNpadButton_RightSL,                                 ///< Bitmask containing SL buttons on both Joy-Cons (Left/Right)
    HidNpadButton_AnySR    = HidNpadButton_LeftSR | HidNpadButton_RightSR,                                 ///< Bitmask containing SR buttons on both Joy-Cons (Left/Right)
} HidNpadButton;

/// HidAnalogStickState
typedef struct HidAnalogStickState {
    s32 x;                                    ///< X
    s32 y;                                    ///< Y
} HidAnalogStickState;

/// Mask including all existing controller IDs.
#define PAD_ANY_ID_MASK 0x1000100FFUL

/// HID controller IDs
typedef enum {
    HidNpadIdType_No1      = 0,    ///< Player 1 controller
    HidNpadIdType_No2      = 1,    ///< Player 2 controller
    HidNpadIdType_No3      = 2,    ///< Player 3 controller
    HidNpadIdType_No4      = 3,    ///< Player 4 controller
    HidNpadIdType_No5      = 4,    ///< Player 5 controller
    HidNpadIdType_No6      = 5,    ///< Player 6 controller
    HidNpadIdType_No7      = 6,    ///< Player 7 controller
    HidNpadIdType_No8      = 7,    ///< Player 8 controller
    HidNpadIdType_Other    = 0x10, ///< Other controller
    HidNpadIdType_Handheld = 0x20, ///< Handheld mode controls
} HidNpadIdType;

/// HidNpadAttribute
typedef enum {
    HidNpadAttribute_IsConnected          = BIT(0),    ///< IsConnected
    HidNpadAttribute_IsWired              = BIT(1),    ///< IsWired
    HidNpadAttribute_IsLeftConnected      = BIT(2),    ///< IsLeftConnected
    HidNpadAttribute_IsLeftWired          = BIT(3),    ///< IsLeftWired
    HidNpadAttribute_IsRightConnected     = BIT(4),    ///< IsRightConnected
    HidNpadAttribute_IsRightWired         = BIT(5),    ///< IsRightWired
} HidNpadAttribute;

/// HID controller styles
typedef enum {
    HidNpadStyleTag_NpadFullKey       = BIT(0),       ///< Pro Controller
    HidNpadStyleTag_NpadHandheld      = BIT(1),       ///< Joy-Con controller in handheld mode
    HidNpadStyleTag_NpadJoyDual       = BIT(2),       ///< Joy-Con controller in dual mode
    HidNpadStyleTag_NpadJoyLeft       = BIT(3),       ///< Joy-Con left controller in single mode
    HidNpadStyleTag_NpadJoyRight      = BIT(4),       ///< Joy-Con right controller in single mode
    HidNpadStyleTag_NpadGc            = BIT(5),       ///< GameCube controller
    HidNpadStyleTag_NpadPalma         = BIT(6),       ///< Poké Ball Plus controller
    HidNpadStyleTag_NpadLark          = BIT(7),       ///< NES/Famicom controller
    HidNpadStyleTag_NpadHandheldLark  = BIT(8),       ///< NES/Famicom controller in handheld mode
    HidNpadStyleTag_NpadLucia         = BIT(9),       ///< SNES controller
    HidNpadStyleTag_Npad10            = BIT(10),
    HidNpadStyleTag_NpadSystemExt     = BIT(29),      ///< Generic external controller
    HidNpadStyleTag_NpadSystem        = BIT(30),      ///< Generic controller

    HidNpadStyleSet_NpadFullCtrl = HidNpadStyleTag_NpadFullKey  | HidNpadStyleTag_NpadHandheld | HidNpadStyleTag_NpadJoyDual,  ///< Style set comprising Npad styles containing the full set of controls {FullKey, Handheld, JoyDual}
    HidNpadStyleSet_NpadStandard = HidNpadStyleSet_NpadFullCtrl | HidNpadStyleTag_NpadJoyLeft  | HidNpadStyleTag_NpadJoyRight, ///< Style set comprising all standard Npad styles {FullKey, Handheld, JoyDual, JoyLeft, JoyRight}
} HidNpadStyleTag;

/// Pad state object.
typedef struct {
    u8 id_mask;
    u8 active_id_mask;
    bool read_handheld;
    bool active_handheld;
    u32 style_set; // HidNpadStyleTag
    u32 attributes; // HidNpadAttribute
    u64 buttons_cur; // HidNpadButton
    u64 buttons_old;
    HidAnalogStickState sticks[2];
    u32 gc_triggers[2];
} PadState;

inline static void padConfigureInput(u32 max_players, u32 style_set) {}

#define padInitialize(_pad, ...) ({ \
    const HidNpadIdType _pad_ids[] = { __VA_ARGS__ }; \
    u64 _pad_mask = 0; \
    for (unsigned _pad_i = 0; _pad_i < (sizeof(_pad_ids)/sizeof(_pad_ids[0])); ++_pad_i) \
        _pad_mask |= 1UL << (_pad_ids[_pad_i]); \
    padInitializeWithMask((_pad), _pad_mask); \
})

inline static void padInitializeWithMask(PadState* pad, u64 mask) {}
inline static void padInitializeAny(PadState* pad) {}
inline static void padInitializeDefault(PadState* pad) {}
inline static void padUpdate(PadState* pad) {}

inline static bool padIsHandheld(const PadState* pad) {
    return pad->active_handheld;
}

inline static bool padIsNpadActive(const PadState* pad, HidNpadIdType id) {
    return false;
}

inline static bool padIsConnected(const PadState* pad) {
    return pad->attributes & HidNpadAttribute_IsConnected;
}

inline static u64 padGetButtons(const PadState* pad) {
    return pad->buttons_cur;
}

inline static HidAnalogStickState padGetStickPos(const PadState* pad, unsigned i) {
    return pad->sticks[i];
}

inline static u32 padGetGcTriggerPos(const PadState* pad, unsigned i) {
    return pad->gc_triggers[i];
}

/// HidVibrationDeviceHandle
typedef union HidVibrationDeviceHandle {
    u32 type_value;                                   ///< TypeValue
    struct {
        u32 npad_style_index : 8;                     ///< NpadStyleIndex
        u32 player_number : 8;                        ///< PlayerNumber
        u32 device_idx : 8;                           ///< DeviceIdx
        u32 pad : 8;                                  ///< Padding
    };
} HidVibrationDeviceHandle;

/// HidVibrationDeviceInfo
typedef struct HidVibrationDeviceInfo {
    u32 type;                                         ///< \ref HidVibrationDeviceType
    u32 position;                                     ///< \ref HidVibrationDevicePosition
} HidVibrationDeviceInfo;

/// HidVibrationValue
typedef struct HidVibrationValue {
    float amp_low;   ///< Low Band amplitude. 1.0f: Max amplitude.
    float freq_low;  ///< Low Band frequency in Hz.
    float amp_high;  ///< High Band amplitude. 1.0f: Max amplitude.
    float freq_high; ///< High Band frequency in Hz.
} HidVibrationValue;

inline static Result hidInitializeVibrationDevices(HidVibrationDeviceHandle *handles, s32 total_handles, HidNpadIdType id, HidNpadStyleTag style) {
    return 0;
}

inline static Result hidSendVibrationValues(const HidVibrationDeviceHandle *handles, const HidVibrationValue *values, s32 count) {
    return 0;
}
