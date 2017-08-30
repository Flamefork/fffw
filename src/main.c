#include "bjdevlib_tb.h"
#include "axefx.h"
#include "lcd_tb.h"

#include <string.h>

#include "axe_default_cc.h"

#define VERSION_STRING "Version 0.05"

// TB hardware consts
#define TB_LCD_WIDTH 16

// Axe-Fx specific config
#define MY_AXEFX_MODEL AXEFX_2_XL_PLUS_MODEL
#define MY_AXEFX_MIDI_CHANNEL 0
#define MY_AXEFX_PRESET_BANK 3

#define CC_MIN_VALUE 0x00
#define CC_MAX_VALUE 0x7F

// Types

typedef enum ButtonType {
  BUTTON_NONE,
  BUTTON_PRESET,
  BUTTON_SCENE,
  BUTTON_IA
} ButtonType;

typedef struct Button {
  ButtonType type;
  uint8_t value;
  LedColor color;
  bool active;
} Button;

// Variables

const char currentPresetName[TB_LCD_WIDTH] = VERSION_STRING;

Button buttons[FOOT_BUTTONS_NUM] = {{.type=BUTTON_PRESET, .color=COLOR_RED, .value=17},
                                    {.type=BUTTON_PRESET, .color=COLOR_RED, .value=18},
                                    {.type=BUTTON_PRESET, .color=COLOR_RED, .value=19},
                                    {.type=BUTTON_PRESET, .color=COLOR_RED, .value=20},
                                    {.type=BUTTON_PRESET, .color=COLOR_RED, .value=21},
                                    {.type=BUTTON_SCENE, .color=COLOR_YELLOW, .value=0},
                                    {.type=BUTTON_SCENE, .color=COLOR_YELLOW, .value=1},
                                    {.type=BUTTON_SCENE, .color=COLOR_YELLOW, .value=2},
                                    {.type=BUTTON_SCENE, .color=COLOR_YELLOW, .value=3},
                                    {.type=BUTTON_IA, .color=COLOR_GREEN, .value=CC_DRIVE_1_BYPASS},
                                    {.type=BUTTON_NONE, .color=COLOR_BLACK},
};

// Indication

void setButtonActive(ButtonType type, uint8_t value, bool isActive, bool deactivateType) {
  for (uint8_t i = 0; i < FOOT_BUTTONS_NUM; i++) {
    if (buttons[i].type == type) {
      if (deactivateType) {
        buttons[i].active = false;
      }
      if (buttons[i].value == value) {
        buttons[i].active = isActive;
      }
    }
  }
}

void updateLeds() {
  for (uint8_t i = 0; i < FOOT_BUTTONS_NUM; i++) {
    LedColor color = buttons[i].active ? buttons[i].color : COLOR_BLACK;
    ledSetColor(i, color, false);
  }

  ledSend();
}

void updateScreen() {
  LCDWriteStringXY(0, 0, "   >> FFFW <<   ");
  LCDWriteStringXY(0, 1, "                ");
  LCDWriteStringXY(0, 1, (const char *) currentPresetName);
}

// Axe-Fx state

void parseIaStates(uint8_t *sysexData) {
  AxeFxEffectBlockState state;
  uint8_t totalEffectsInMessage = axefxGetEffectBlockStateNumber(sysexData);

  for (uint8_t i = 0; i < totalEffectsInMessage; ++i) {
    if (axefxGetSingleEffectBlockState(&state, i, sysexData)) {
      for (uint8_t j = 0; j < FOOT_BUTTONS_NUM; j++) {
        if (buttons[j].type == BUTTON_IA && buttons[j].value == state.iaCcNumber_) {
          buttons[j].active = state.isEnabled_;
        }
      }
    }
  }
}

// Actions

void buttonsCallback(ButtonEvent buttonEvent) {
  if (buttonEvent.actionType_ == BUTTON_NO_EVENT) {
    return;
  }

  if (buttonEvent.actionType_ == BUTTON_PUSH) {
    Button button = buttons[buttonEvent.buttonNum_];

    switch (button.type) {
      case BUTTON_PRESET:
        setButtonActive(BUTTON_PRESET, button.value, true, true);
        setButtonActive(BUTTON_SCENE, 0, true, true);

        midiSendControlChange(0, MY_AXEFX_PRESET_BANK, MY_AXEFX_MIDI_CHANNEL);
        midiSendProgramChange(button.value, MY_AXEFX_MIDI_CHANNEL);
        axefxSendFunctionRequest(MY_AXEFX_MODEL, AXEFX_GET_PRESET_BLOCKS_FLAGS, NULL, 0);

        updateLeds();
        break;

      case BUTTON_SCENE:
        setButtonActive(BUTTON_SCENE, button.value, true, true);

        midiSendControlChange(CC_SCENE_SELECT, button.value, MY_AXEFX_MIDI_CHANNEL);
        axefxSendFunctionRequest(MY_AXEFX_MODEL, AXEFX_GET_PRESET_BLOCKS_FLAGS, NULL, 0);

        updateLeds();
        break;

      case BUTTON_IA:
        setButtonActive(BUTTON_IA, button.value, !button.active, false);

        midiSendControlChange(button.value, button.active ? CC_MIN_VALUE : CC_MAX_VALUE, MY_AXEFX_MIDI_CHANNEL);

        updateLeds();
        break;

      case BUTTON_NONE:
      default:
        break;
    }
  }
}

void sysExCallback(uint16_t length) {
  uint8_t *sysexData = midiGetLastSysExData();

  if (!axeFxCheckFractalManufId(sysexData)) {
    return;
  }

  AxeFxFunctionId function = axeFxGetFunctionId(sysexData);

  switch (function) {
    case AXEFX_GET_PRESET_BLOCKS_FLAGS:
      parseIaStates(sysexData);
      axefxSendFunctionRequest(MY_AXEFX_MODEL, AXEFX_GET_PRESET_NAME, NULL, 0);

      updateLeds();
      break;

    case AXEFX_GET_PRESET_NAME:
      axefxGetPresetName((char *) currentPresetName, TB_LCD_WIDTH, sysexData);

      updateScreen();
      break;

    default:
      break;
  }
}

// Main

int main(void) {
  initBjDevLib();

  LCDInit(LS_NONE);
//  LcdHideCursor();

  midiRegisterSysExCallback(sysExCallback);

  updateLeds();
  updateScreen();

  while (1) {
    buttonsCallback(getButtonLastEvent());
    midiRead();
  }
}
