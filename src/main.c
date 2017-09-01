#include "bjdevlib_tb.h"
#include "axefx.h"
#include "lcd_tb.h"

#include <string.h>

#include "axe_defs.h"

#define VERSION_STRING "Version 0.05"

// TB hardware consts
#define TB_LCD_WIDTH 16
#define PEDAL_LED 10
#define EXP_PEDALS_NUM 3

// Axe-Fx specific config
#define MY_AXEFX_MODEL AXEFX_2_XL_PLUS_MODEL
#define MY_AXEFX_MIDI_CHANNEL 0

#define CC_MIN_VALUE 0x00
#define CC_MAX_VALUE 0x7F

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
  uint8_t       value2;
  LedColor      color;
  PedalLedColor pedalColor;
  bool active;
} Button;

typedef struct ExpressionPedal {
  uint8_t calibrationMin;
  uint8_t calibrationMax;
  uint8_t ccNumber;
} ExpressionPedal;

// Variables

const char currentPresetName[TB_LCD_WIDTH + 1] = VERSION_STRING;

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

// Indication

void setButtonActive(ButtonType type, uint16_t value, bool isActive, bool deactivateType) {
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
    if (i != PEDAL_LED) {
      LedColor color = buttons[i].active ? buttons[i].color : COLOR_BLACK;
      ledSetColor(i, color, false);
    } else {
      PedalLedColor color = buttons[i].active ? buttons[i].pedalColor : PEDAL_COLOR_NO;
      ledSetPedalColorAll(PEDAL_COLOR_NO, false);
      ledSetPedalColor(0, color, false);
      ledSetPedalColor(8, color, false);
    }
  }

  ledSend();
  ledPedalSend();
}

void updateScreen() {
  LCDWriteStringXY(0, 0, "   >> FFFW <<   ");
  LCDWriteStringXY(0, 1, "                ");
  LCDWriteStringXY(0, 1, (const char *) currentPresetName);
}

// Axe-Fx state

void parseBlockInfo(uint8_t *sysexData) {
  AxeFxEffectBlockState state;

  uint8_t totalEffectsInMessage = axefxGetEffectBlockStateNumber(sysexData);

  for (uint8_t j = 0; j < FOOT_BUTTONS_NUM; j++) {
    if (buttons[j].type == BUTTON_BLOCK_BYPASS) {
      buttons[j].value2 = 0;
      buttons[j].active = false;
    }
  }
  for (uint8_t i = 0; i < totalEffectsInMessage; ++i) {
    if (axefxGetSingleEffectBlockState(&state, i, sysexData)) {
      for (uint8_t j = 0; j < FOOT_BUTTONS_NUM; j++) {
        if (buttons[j].type == BUTTON_BLOCK_BYPASS && buttons[j].value == state.effectId_) {
          buttons[j].value2 = state.iaCcNumber_;
          buttons[j].active = state.isEnabled_;
        }
      }
    }
  }
}

// Callbacks

void buttonsCallback(ButtonEvent buttonEvent) {
  if (buttonEvent.actionType_ == BUTTON_NO_EVENT) {
    return;
  }
  LOG(SEV_INFO, "Button %d event: %d", buttonEvent.buttonNum_, buttonEvent.actionType_);

  if (buttonEvent.actionType_ == BUTTON_PUSH) {
    Button button = buttons[buttonEvent.buttonNum_];

    switch (button.type) {
      case BUTTON_PRESET:
        midiSendControlChange(0, button.value / 128, MY_AXEFX_MIDI_CHANNEL);
        LOG(SEV_INFO, "SENT Bank select: %d", button.value / 128);
        midiSendProgramChange(button.value % 128, MY_AXEFX_MIDI_CHANNEL);
        LOG(SEV_INFO, "SENT Program change: %d", button.value % 128);
        break;

      case BUTTON_SCENE:
        midiSendControlChange(CC_SCENE_SELECT, button.value, MY_AXEFX_MIDI_CHANNEL);
        LOG(SEV_INFO, "SENT Scene select: %d", CC_SCENE_SELECT);
        break;

      case BUTTON_BLOCK_BYPASS:
        if (!button.value2) {
          LOG(SEV_WARNING, "Unknown CC for block %d", button.value2);
          break;
        }
        uint8_t ccValue = button.active ? CC_MIN_VALUE : CC_MAX_VALUE;
        midiSendControlChange(button.value2, ccValue, MY_AXEFX_MIDI_CHANNEL);
        LOG(SEV_INFO, "SENT Control change: %d = %d", button.value2, ccValue);
        axefxSendFunctionRequest(MY_AXEFX_MODEL, AXEFX_GET_PRESET_BLOCKS_FLAGS, NULL, 0);
        LOG(SEV_INFO, "SENT AXEFX_GET_PRESET_BLOCKS_FLAGS");
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
      LOG(SEV_INFO, "GOT  AXEFX_GET_PRESET_BLOCKS_FLAGS");

      parseBlockInfo(sysexData);

      updateLeds();
      break;

    case AXEFX_GET_PRESET_NAME:
      LOG(SEV_INFO, "GOT  AXEFX_GET_PRESET_NAME");

      axefxGetPresetName((char *) currentPresetName, TB_LCD_WIDTH + 1, sysexData);

      updateScreen();
      break;

    case AXEFX_GET_PRESET_NUMBER:
      LOG(SEV_INFO, "GOT  AXEFX_GET_PRESET_NUMBER");

      uint16_t presetNumber = axefxGetPresetNumber(sysexData);
      setButtonActive(BUTTON_PRESET, presetNumber, true, true);

      axefxSendFunctionRequest(MY_AXEFX_MODEL, AXEFX_GET_PRESET_NAME, NULL, 0);
      LOG(SEV_INFO, "SENT AXEFX_GET_PRESET_NAME");
      axefxSendFunctionRequest(MY_AXEFX_MODEL, AXEFX_GET_PRESET_BLOCKS_FLAGS, NULL, 0);
      LOG(SEV_INFO, "SENT AXEFX_GET_PRESET_BLOCKS_FLAGS");

      updateLeds();
      break;

    case AXEFX_SET_SCENE_NUMBER:
      LOG(SEV_INFO, "GOT  AXEFX_SET_SCENE_NUMBER");

      uint8_t sceneNumber = axefxGetSceneNumber(sysexData);
      setButtonActive(BUTTON_SCENE, sceneNumber, true, true);

      updateLeds();
      break;

    case AXEFX_FRONT_PANEL_CHANGE_DETECTED:
      LOG(SEV_INFO, "GOT  AXEFX_FRONT_PANEL_CHANGE_DETECTED");

      axefxSendFunctionRequest(MY_AXEFX_MODEL, AXEFX_GET_PRESET_BLOCKS_FLAGS, NULL, 0);
      LOG(SEV_INFO, "SENT AXEFX_GET_PRESET_BLOCKS_FLAGS");
      break;

    case AXEFX_TEMPO_BEAT:
      //LOG(SEV_INFO, "GOT  AXEFX_TEMPO_BEAT");
      break;

    default:
      LOG(SEV_INFO, "GOT  Unhandled function response: %02X", function);
      break;
  }
}

void expPedalsCallback(PedalNumber pedalNumber, uint8_t pedalPosition) {
  LOG(SEV_INFO, "Exp pedal %d position change: %d", pedalNumber, pedalPosition);

  ExpressionPedal exp = expressionPedals[pedalNumber];
  if (exp.ccNumber) {
    int8_t value = 127 * (pedalPosition - exp.calibrationMin) / (exp.calibrationMax - exp.calibrationMin);
    value = value < CC_MIN_VALUE ? CC_MIN_VALUE : value > CC_MAX_VALUE ? CC_MAX_VALUE : value;
    midiSendControlChange(exp.ccNumber, (uint8_t) value, MY_AXEFX_MIDI_CHANNEL);
    LOG(SEV_INFO, "SENT Control change: %d = %d", exp.ccNumber, value);
  }
}

// Main

int main(void) {
  initBjDevLib();

  LCDInit(LS_NONE);
  LcdHideCursor();

  midiRegisterSysExCallback(sysExCallback);

  axefxSendFunctionRequest(MY_AXEFX_MODEL, AXEFX_GET_PRESET_NUMBER, NULL, 0);
  LOG(SEV_INFO, "SENT AXEFX_GET_PRESET_NUMBER");

  expRegisterPedalChangePositionCallback(expPedalsCallback);

  updateLeds();
  updateScreen();

  while (1) {
    buttonsCallback(getButtonLastEvent());
    expProcess();
    midiRead();
  }
}
