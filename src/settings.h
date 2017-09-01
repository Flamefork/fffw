// Version
#define VERSION_STRING "Version 0.2"

// TB hardware consts
#define TB_LCD_WIDTH 16
#define PEDAL_LED 10
#define EXP_PEDALS_NUM 3

// Axe-Fx specific config
#define MY_AXEFX_MODEL AXEFX_2_XL_PLUS_MODEL
#define MY_AXEFX_MIDI_CHANNEL 0

// Types

typedef enum ButtonType {
  BUTTON_NONE,
  BUTTON_PRESET,
  BUTTON_SCENE,
  BUTTON_BLOCK_BYPASS
} ButtonType;

typedef struct Button {
  ButtonType    type;
  uint16_t      value;
  LedColor      color;
  PedalLedColor pedalColor;
} Button;

typedef struct ExpressionPedal {
  uint8_t calibrationMin;
  uint8_t calibrationMax;
  uint8_t ccNumber;
} ExpressionPedal;

// Variables

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
