#include "common_defs.h"

Button buttons[FOOT_BUTTONS_NUM] = {{.type=BUTTON_PRESET, .color=COLOR_RED, .value=401},
                                    {.type=BUTTON_PRESET, .color=COLOR_RED, .value=402},
                                    {.type=BUTTON_PRESET, .color=COLOR_RED, .value=403},
                                    {.type=BUTTON_PRESET, .color=COLOR_RED, .value=404},
                                    {.type=BUTTON_PRESET, .color=COLOR_RED, .value=405},
                                    {.type=BUTTON_SCENE, .color=COLOR_YELLOW, .value=0},
                                    {.type=BUTTON_SCENE, .color=COLOR_YELLOW, .value=1},
                                    {.type=BUTTON_SCENE, .color=COLOR_YELLOW, .value=2},
                                    {.type=BUTTON_SCENE, .color=COLOR_YELLOW, .value=3},
                                    {.type=BUTTON_BLOCK_BYPASS, .color=COLOR_GREEN, .value=AXEFX_BLOCK_DRIVE_1},
                                    {.type=BUTTON_BLOCK_BYPASS, .pedalColor=PEDAL_COLOR_G, .value=AXEFX_BLOCK_WAH_1}};

ExpressionPedal expressionPedals[EXP_PEDALS_NUM] = {{.calibrationMin=0, .calibrationMax=127, .ccNumber=17},
                                                    {.calibrationMin=0, .calibrationMax=127, .ccNumber=18},
                                                    {.calibrationMin=13, .calibrationMax=112, .ccNumber=16}};
