#include "common_defs.h"

#define BUTTON_PAGE_PRESETS 0
#define BUTTON_PAGE_EFFECTS 1
#define BUTTON_PAGE_LOOPER 2

Button presetsPage[FOOT_BUTTONS_NUM] = {
    /* 1 */{.type=BUTTON_SCENE, .color=COLOR_YELLOW, .altColor=COLOR_RED, .value=1, .altValue=5},
    /* 2 */{.type=BUTTON_SCENE, .color=COLOR_YELLOW, .altColor=COLOR_RED, .value=2, .altValue=6},
    /* 3 */{.type=BUTTON_SCENE, .color=COLOR_YELLOW, .altColor=COLOR_RED, .value=3, .altValue=7},
    /* 4 */{.type=BUTTON_SCENE, .color=COLOR_YELLOW, .altColor=COLOR_RED, .value=4, .altValue=8},
    /* 5 */{.type=BUTTON_PAGE, .color=COLOR_GREEN, .value=BUTTON_PAGE_EFFECTS, .altValue=BUTTON_PAGE_LOOPER},

    /* 6 */{.type=BUTTON_PRESET, .color=COLOR_YELLOW, .altColor=COLOR_RED, .value=401, .altValue=400},
    /* 7 */{.type=BUTTON_PRESET, .color=COLOR_YELLOW, .altColor=COLOR_RED, .value=403, .altValue=402},
    /* 8 */{.type=BUTTON_PRESET, .color=COLOR_YELLOW, .altColor=COLOR_RED, .value=405, .altValue=404},
    /* 9 */{.type=BUTTON_PRESET, .color=COLOR_YELLOW, .altColor=COLOR_RED, .value=407, .altValue=406},
    /*10 */{.type=BUTTON_PRESET, .color=COLOR_YELLOW, .altColor=COLOR_RED, .value=409, .altValue=408},

    /* P */{.type=BUTTON_BLOCK_BYPASS, .pedalColor=PEDAL_COLOR_RGB, .value=AXEFX_BLOCK_VOLUME_PAN_1}};

Button effectsPage[FOOT_BUTTONS_NUM] = {
    /* 1 */{.type=BUTTON_BLOCK_BYPASS, .color=COLOR_GREEN, .altColor=COLOR_YELLOW, .value=AXEFX_BLOCK_DRIVE_1},
    /* 2 */{.type=BUTTON_BLOCK_BYPASS, .color=COLOR_GREEN, .altColor=COLOR_YELLOW, .value=AXEFX_BLOCK_DELAY_1},
    /* 3 */{.type=BUTTON_BLOCK_BYPASS, .color=COLOR_GREEN, .altColor=COLOR_YELLOW, .value=AXEFX_BLOCK_PITCH_1, .momentary=true},
    /* 4 */{.type=BUTTON_TAP_TEMPO, .color=COLOR_GREEN},
    /* 5 */{.type=BUTTON_PAGE, .color=COLOR_RED, .value=BUTTON_PAGE_PRESETS, .altValue=BUTTON_PAGE_LOOPER},

    /* 6 */{.type=BUTTON_BLOCK_XY, .color=COLOR_GREEN, .altColor=COLOR_YELLOW, .value=AXEFX_BLOCK_AMP_1},
    /* 7 */{.type=BUTTON_BLOCK_XY, .color=COLOR_GREEN, .altColor=COLOR_YELLOW, .value=AXEFX_BLOCK_CAB_1},
    /* 8 */{.type=BUTTON_BLOCK_BYPASS, .color=COLOR_GREEN, .altColor=COLOR_YELLOW, .value=AXEFX_BLOCK_COMPRESSOR_1},
    /* 9 */{.type=BUTTON_BLOCK_BYPASS, .color=COLOR_GREEN, .altColor=COLOR_YELLOW, .value=AXEFX_BLOCK_CHORUS_1},
    /*10 */{.type=BUTTON_BLOCK_BYPASS, .color=COLOR_GREEN, .altColor=COLOR_YELLOW, .value=AXEFX_BLOCK_WAH_1},

    /* P */{.type=BUTTON_BLOCK_BYPASS, .pedalColor=PEDAL_COLOR_RGB, .value=AXEFX_BLOCK_VOLUME_PAN_1}};

Button looperPage[FOOT_BUTTONS_NUM] = {
    /* 1 */{.type=BUTTON_LOOPER, .color=COLOR_GREEN, .value=AXEFX_LOOPER_BIT_RECORD},
    /* 2 */{.type=BUTTON_LOOPER, .color=COLOR_GREEN, .value=AXEFX_LOOPER_BIT_PLAY},
    /* 3 */{.type=BUTTON_LOOPER, .color=COLOR_GREEN, .value=AXEFX_LOOPER_BIT_OVERDUB},
    /* 4 */{.type=BUTTON_TAP_TEMPO, .color=COLOR_GREEN},
    /* 5 */{.type=BUTTON_PAGE, .color=COLOR_YELLOW, .value=BUTTON_PAGE_PRESETS, .altValue=BUTTON_PAGE_LOOPER},

    /* 6 */{.type=BUTTON_LOOPER, .color=COLOR_GREEN, .value=AXEFX_LOOPER_BIT_UNDO},
    /* 7 */{.type=BUTTON_LOOPER, .color=COLOR_GREEN, .value=AXEFX_LOOPER_BIT_REVERSE},
    /* 8 */{.type=BUTTON_LOOPER, .color=COLOR_GREEN, .value=AXEFX_LOOPER_BIT_HALF},
    /* 9 */{.type=BUTTON_LOOPER, .color=COLOR_GREEN, .value=AXEFX_LOOPER_BIT_ONCE},
    /*10 */{.type=BUTTON_BLOCK_BYPASS, .color=COLOR_BLACK, .altColor=COLOR_GREEN, .value=AXEFX_BLOCK_LOOPER},

    /* P */{.type=BUTTON_BLOCK_BYPASS, .pedalColor=PEDAL_COLOR_RGB, .value=AXEFX_BLOCK_VOLUME_PAN_1}};


ButtonPage pages[3] = {{.name="PRESET/SCENE    ", .buttons=presetsPage},
                       {.name="EFFECTS         ", .buttons=effectsPage},
                       {.name="LOOPER          ", .buttons=looperPage}};

ExpressionPedal expressionPedals[EXP_PEDALS_NUM] = {
    // External
    {},
    {},
    // Onboard
    {.ccNumber=CC_EXTERNAL_CONTROL_1, .calibrationMin=13, .calibrationMax=112, .toggleTunerBelow=3}};
