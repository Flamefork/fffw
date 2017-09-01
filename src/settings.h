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
    {.type=BUTTON_SCENE, .color=COLOR_GREEN, .value=0},
    {.type=BUTTON_SCENE, .color=COLOR_GREEN, .value=1},
    {.type=BUTTON_SCENE, .color=COLOR_GREEN, .value=2},
    {.type=BUTTON_SCENE, .color=COLOR_GREEN, .value=3},
    // 5
    {.type=BUTTON_PAGE, .color=COLOR_RED, .value=0},
    // 6 7 8
    {.type=BUTTON_BLOCK_BYPASS, .color=COLOR_YELLOW, .value=AXEFX_BLOCK_DRIVE_1},
    {.type=BUTTON_BLOCK_BYPASS, .color=COLOR_YELLOW, .value=AXEFX_BLOCK_DRIVE_2},
    {.type=BUTTON_BLOCK_BYPASS, .color=COLOR_YELLOW, .value=AXEFX_BLOCK_DELAY_1},
    // 9 10
    {.type=BUTTON_NONE, .color=COLOR_BLACK}, // TODO: Metronome
    {.type=BUTTON_NONE, .color=COLOR_BLACK}, // TODO: Tap tempo
    // P
    {.type=BUTTON_BLOCK_BYPASS, .pedalColor=PEDAL_COLOR_G, .value=AXEFX_BLOCK_WAH_1}};

ButtonPage pages[2] = {{.name="PRESETS / SCENES", .buttons=page0},
                       {.name="EFFECTS / LOOPER", .buttons=page1}};

ExpressionPedal expressionPedals[EXP_PEDALS_NUM] = {
    // External
    {.ccNumber=17, .calibrationMin=0, .calibrationMax=127},
    {.ccNumber=18, .calibrationMin=0, .calibrationMax=127},
    // Onboard
    {.ccNumber=16, .calibrationMin=13, .calibrationMax=112}};
