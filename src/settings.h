#include "common_defs.h"

#define BUTTON_PAGE_PRESETS 0
#define BUTTON_PAGE_LOOPER 1
#define BUTTON_PAGE_EFFECTS 2

Button page0[FOOT_BUTTONS_NUM] = {
    // 1 2 3 4
    {.type=BUTTON_SCENE, .color=COLOR_YELLOW, .altColor=COLOR_RED, .value=1, .altValue=5},
    {.type=BUTTON_SCENE, .color=COLOR_YELLOW, .altColor=COLOR_RED, .value=2, .altValue=6},
    {.type=BUTTON_SCENE, .color=COLOR_YELLOW, .altColor=COLOR_RED, .value=3, .altValue=7},
    {.type=BUTTON_SCENE, .color=COLOR_YELLOW, .altColor=COLOR_RED, .value=4, .altValue=8},
    // 5
    {.type=BUTTON_PAGE, .color=COLOR_GREEN, .value=BUTTON_PAGE_LOOPER, .altValue=BUTTON_PAGE_EFFECTS},
    // 6 7 8 9 10
    {.type=BUTTON_PRESET, .color=COLOR_YELLOW, .altColor=COLOR_RED, .value=401, .altValue=400},
    {.type=BUTTON_PRESET, .color=COLOR_YELLOW, .altColor=COLOR_RED, .value=402, .altValue=409},
    {.type=BUTTON_PRESET, .color=COLOR_YELLOW, .altColor=COLOR_RED, .value=403, .altValue=412},
    {.type=BUTTON_PRESET, .color=COLOR_YELLOW, .altColor=COLOR_RED, .value=404, .altValue=410},
    {.type=BUTTON_PRESET, .color=COLOR_YELLOW, .altColor=COLOR_RED, .value=405, .altValue=419},
    // P
    {.type=BUTTON_BLOCK_BYPASS, .pedalColor=PEDAL_COLOR_RGB, .value=AXEFX_BLOCK_VOLUME_PAN_1}};

Button page1[FOOT_BUTTONS_NUM] = {
    // 1 2 3 4
    {.type=BUTTON_LOOPER, .color=COLOR_GREEN, .value=AXEFX_LOOPER_BIT_RECORD},
    {.type=BUTTON_LOOPER, .color=COLOR_GREEN, .value=AXEFX_LOOPER_BIT_PLAY},
    {.type=BUTTON_LOOPER, .color=COLOR_GREEN, .value=AXEFX_LOOPER_BIT_OVERDUB},
    {.type=BUTTON_TAP_TEMPO, .color=COLOR_GREEN},
    // 5
    {.type=BUTTON_PAGE, .color=COLOR_YELLOW, .value=BUTTON_PAGE_PRESETS, .altValue=BUTTON_PAGE_EFFECTS},
    // 6 7 8 9 10
    {.type=BUTTON_LOOPER, .color=COLOR_GREEN, .value=AXEFX_LOOPER_BIT_UNDO},
    {.type=BUTTON_LOOPER, .color=COLOR_GREEN, .value=AXEFX_LOOPER_BIT_REVERSE},
    {.type=BUTTON_LOOPER, .color=COLOR_GREEN, .value=AXEFX_LOOPER_BIT_HALF},
    {.type=BUTTON_LOOPER, .color=COLOR_GREEN, .value=AXEFX_LOOPER_BIT_ONCE},
    {.type=BUTTON_BLOCK_BYPASS, .color=COLOR_BLACK, .altColor=COLOR_GREEN, .value=AXEFX_BLOCK_LOOPER},

    // P
    {.type=BUTTON_BLOCK_BYPASS, .pedalColor=PEDAL_COLOR_RGB, .value=AXEFX_BLOCK_VOLUME_PAN_1}};

Button page2[FOOT_BUTTONS_NUM] = {
    // 1 2 3 4
    {.type=BUTTON_BLOCK_BYPASS, .color=COLOR_GREEN, .altColor=COLOR_YELLOW, .value=AXEFX_BLOCK_DRIVE_1},
    {.type=BUTTON_BLOCK_BYPASS, .color=COLOR_GREEN, .altColor=COLOR_YELLOW, .value=AXEFX_BLOCK_DELAY_1},
    {.type=BUTTON_BLOCK_BYPASS, .color=COLOR_GREEN, .altColor=COLOR_YELLOW, .value=AXEFX_BLOCK_PITCH_1},
    {.type=BUTTON_TAP_TEMPO, .color=COLOR_GREEN},
    // 5
    {.type=BUTTON_PAGE, .color=COLOR_RED, .value=BUTTON_PAGE_PRESETS},
    // 6 7 8 9 10
    {.type=BUTTON_BLOCK_XY, .color=COLOR_GREEN, .altColor=COLOR_YELLOW, .value=AXEFX_BLOCK_AMP_1},
    {.type=BUTTON_BLOCK_XY, .color=COLOR_GREEN, .altColor=COLOR_YELLOW, .value=AXEFX_BLOCK_CAB_1},
    {.type=BUTTON_BLOCK_BYPASS, .color=COLOR_GREEN, .altColor=COLOR_YELLOW, .value=AXEFX_BLOCK_COMPRESSOR_1},
    {.type=BUTTON_BLOCK_BYPASS, .color=COLOR_GREEN, .altColor=COLOR_YELLOW, .value=AXEFX_BLOCK_CHORUS_1},
    {.type=BUTTON_BLOCK_BYPASS, .color=COLOR_GREEN, .altColor=COLOR_YELLOW, .value=AXEFX_BLOCK_WAH_1},

    // P
    {.type=BUTTON_BLOCK_BYPASS, .pedalColor=PEDAL_COLOR_RGB, .value=AXEFX_BLOCK_VOLUME_PAN_1}};

ButtonPage pages[3] = {{.name="PRESET/SCENE    ", .buttons=page0},
                       {.name="LOOPER          ", .buttons=page1, .showLooperStatus=true},
                       {.name="EFFECTS         ", .buttons=page2}};

ExpressionPedal expressionPedals[EXP_PEDALS_NUM] = {
    // External
    {},
    {},
    // Onboard
    {.ccNumber=CC_EXTERNAL_CONTROL_1, .calibrationMin=13, .calibrationMax=112, .toggleTunerBelow=3}};
