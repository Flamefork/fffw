#include "bjdevlib_tb.h"
#include "axefx.h"
#include "lcd_tb.h"

#include <string.h>

#include "axe_defs.h"
#include "settings.h"

// Axe-Fx state

char                  presetName[TB_LCD_WIDTH + 1] = VERSION_STRING;
uint16_t              presetNumber                 = 0;
uint8_t               sceneNumber                  = 0;
AxeFxEffectBlockState blockStates[0xFF];

void sendCC(uint8_t ctrlNum, uint8_t val) {
  midiSendControlChange(ctrlNum, val, MY_AXEFX_MIDI_CHANNEL);
  LOG(SEV_INFO, "SENT CC: %d = %d", ctrlNum, val);
}
void sendPC(uint8_t progNum) {
  midiSendProgramChange(progNum, MY_AXEFX_MIDI_CHANNEL);
  LOG(SEV_INFO, "SENT PC: %d", progNum);
}
void sendFX(AxeFxFunctionId functionId) {
  axefxSendFunctionRequest(MY_AXEFX_MODEL, functionId, NULL, 0);
  LOG(SEV_INFO, "SENT FX: %02X", functionId);
}

void parseBlockInfo(uint8_t *sysexData) {
  uint8_t totalEffectsInMessage = axefxGetEffectBlockStateNumber(sysexData);

  memset(&blockStates, 0, sizeof blockStates);
  for (uint8_t i = 0; i < totalEffectsInMessage; ++i) {
    AxeFxEffectBlockState state;
    if (axefxGetSingleEffectBlockState(&state, i, sysexData)) {
      blockStates[state.effectId_] = state;
    }
  }
}

void setPresetNumber(uint16_t number) {
  sendCC(CC_BANK_CHANGE, number / 128);
  sendPC(number % 128);
}

void setSceneNumber(uint8_t number) {
  sendCC(CC_SCENE_SELECT, number);
}

void toggleBlock(uint8_t blockId) {
  if (!blockStates[blockId].iaCcNumber_) {
    LOG(SEV_WARNING, "Unknown CC for block %d", blockId);
    return;
  }
  uint8_t ccValue = blockStates[blockId].isEnabled_ ? CC_MIN_VALUE : CC_MAX_VALUE;
  sendCC(blockStates[blockId].iaCcNumber_, ccValue);

  sendFX(AXEFX_GET_PRESET_BLOCKS_FLAGS);
}

// Indication

void updateLeds() {
  for (uint8_t i = 0; i < FOOT_BUTTONS_NUM; i++) {
    bool isActive = false;
    switch (buttons[i].type) {
      case BUTTON_PRESET:
        isActive = buttons[i].value == presetNumber;
        break;
      case BUTTON_SCENE:
        isActive = buttons[i].value == sceneNumber;
        break;
      case BUTTON_BLOCK_BYPASS:
        isActive = blockStates[buttons[i].value].isEnabled_;
        break;
      case BUTTON_NONE:
      default:
        break;
    }

    if (i != PEDAL_LED) {
      LedColor color = isActive ? buttons[i].color : COLOR_BLACK;
      ledSetColor(i, color, false);
    } else {
      PedalLedColor color = isActive ? buttons[i].pedalColor : PEDAL_COLOR_NO;
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
  LCDWriteStringXY(0, 1, (const char *) presetName);
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
        setPresetNumber(button.value);
        break;
      case BUTTON_SCENE:
        setSceneNumber(button.value);
        break;
      case BUTTON_BLOCK_BYPASS:
        toggleBlock(button.value);
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
  LOG(SEV_INFO, "GOT FX: %02X", function);

  bool shouldUpdate = false;

  switch (function) {
    case AXEFX_GET_PRESET_BLOCKS_FLAGS:
      parseBlockInfo(sysexData);
      shouldUpdate = true;
      break;

    case AXEFX_GET_PRESET_NAME:
      axefxGetPresetName((char *) presetName, TB_LCD_WIDTH + 1, sysexData);
      shouldUpdate = true;
      break;

    case AXEFX_GET_PRESET_NUMBER:
      presetNumber = axefxGetPresetNumber(sysexData);
      shouldUpdate = true;

      sendFX(AXEFX_GET_PRESET_NAME);
      sendFX(AXEFX_GET_PRESET_BLOCKS_FLAGS);
      break;

    case AXEFX_SET_SCENE_NUMBER:
      sceneNumber = axefxGetSceneNumber(sysexData);
      shouldUpdate = true;
      break;

    case AXEFX_FRONT_PANEL_CHANGE_DETECTED:
      sendFX(AXEFX_GET_PRESET_BLOCKS_FLAGS);
      break;

    case AXEFX_TEMPO_BEAT:
      break;

    default:
      LOG(SEV_WARNING, "Unhandled FX: %02X", function);
      break;
  }

  if (shouldUpdate) {
    updateLeds();
    updateScreen();
  }
}

void expPedalsCallback(PedalNumber pedalNumber, uint8_t pedalPosition) {
  LOG(SEV_INFO, "Exp pedal %d position change: %d", pedalNumber, pedalPosition);

  ExpressionPedal exp = expressionPedals[pedalNumber];
  if (exp.ccNumber) {
    int8_t value = 127 * (pedalPosition - exp.calibrationMin) / (exp.calibrationMax - exp.calibrationMin);
    value = value < CC_MIN_VALUE ? CC_MIN_VALUE : value > CC_MAX_VALUE ? CC_MAX_VALUE : value;
    sendCC(exp.ccNumber, (uint8_t) value);
  }
}

// Main

int main(void) {
  initBjDevLib();

  LCDInit(LS_NONE);
  LcdHideCursor();

  midiRegisterSysExCallback(sysExCallback);

  sendFX(AXEFX_GET_PRESET_NUMBER);

  expRegisterPedalChangePositionCallback(expPedalsCallback);

  updateLeds();
  updateScreen();

  while (1) {
    buttonsCallback(getButtonLastEvent());
    expProcess();
    midiRead();
  }
}
