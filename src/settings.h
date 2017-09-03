#include "common_defs.h"

Button page0[FOOT_BUTTONS_NUM] = {
    // 1 2 3 4
    {.type=BUTTON_SCENE, .color=COLOR_YELLOW, .value=0},
    {.type=BUTTON_SCENE, .color=COLOR_YELLOW, .value=1},
    {.type=BUTTON_SCENE, .color=COLOR_YELLOW, .value=2},
    {.type=BUTTON_SCENE, .color=COLOR_YELLOW, .value=3},
    // 5
    {.type=BUTTON_PAGE, .color=COLOR_GREEN, .value=1},
    // 6 7 8 9 10
    {.type=BUTTON_PRESET, .color=COLOR_RED, .value=401},
    {.type=BUTTON_PRESET, .color=COLOR_RED, .value=402},
    {.type=BUTTON_PRESET, .color=COLOR_RED, .value=403},
    {.type=BUTTON_PRESET, .color=COLOR_RED, .value=404},
    {.type=BUTTON_PRESET, .color=COLOR_RED, .value=405},
    // P
    {.type=BUTTON_BLOCK_BYPASS, .pedalColor=PEDAL_COLOR_G, .value=AXEFX_BLOCK_WAH_1}};

Button page1[FOOT_BUTTONS_NUM] = {
    // 1 2 3 4
    {.type=BUTTON_LOOPER, .color=COLOR_GREEN, .value=AXEFX_LOOPER_BIT_RECORD},
    {.type=BUTTON_LOOPER, .color=COLOR_GREEN, .value=AXEFX_LOOPER_BIT_PLAY},
    {.type=BUTTON_LOOPER, .color=COLOR_GREEN, .value=AXEFX_LOOPER_BIT_OVERDUB},
    {.type=BUTTON_LOOPER, .color=COLOR_GREEN, .value=AXEFX_LOOPER_BIT_UNDO},
    // 5
    {.type=BUTTON_PAGE, .color=COLOR_RED, .value=0},
    // 6 7 8 9 10
    {.type=BUTTON_BLOCK_BYPASS, .color=COLOR_YELLOW, .value=AXEFX_BLOCK_DRIVE_1},
    {.type=BUTTON_BLOCK_BYPASS, .color=COLOR_YELLOW, .value=AXEFX_BLOCK_DELAY_1},
    {.type=BUTTON_NONE, .color=COLOR_BLACK},
    {.type=BUTTON_NONE, .color=COLOR_BLACK},
    {.type=BUTTON_NONE, .color=COLOR_BLACK},
    // P
    {.type=BUTTON_BLOCK_BYPASS, .pedalColor=PEDAL_COLOR_G, .value=AXEFX_BLOCK_WAH_1}};

ButtonPage pages[2] = {{.name="PRESETS / SCENES", .buttons=page0},
                       {.name="EFFECTS / LOOPER", .buttons=page1}};

ExpressionPedal expressionPedals[EXP_PEDALS_NUM] = {
    // External
    {},
    {},
    // Onboard
    {.ccNumber=16, .calibrationMin=13, .calibrationMax=112, .toggleTunerBelow=3}};
