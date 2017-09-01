#include "bjdevlib_tb.h"
#include "lcd_tb.h"

#include "axe_state.h"
#include "settings.h"

// Indication

void updateLeds() {
  for (uint8_t i = 0; i < FOOT_BUTTONS_NUM; i++) {
    bool isActive = false;
    switch (buttons[i].type) {
      case BUTTON_PRESET:
        isActive = buttons[i].value == axeGetPresetNumber();
        break;
      case BUTTON_SCENE:
        isActive = buttons[i].value == axeGetSceneNumber();
        break;
      case BUTTON_BLOCK_BYPASS:
        isActive = axeGetBlockActive(buttons[i].value);
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
  LCDWriteStringXY(0, 1, axeGetPresetName());
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
        axeSetPresetNumber(button.value);
        break;
      case BUTTON_SCENE:
        axeSetSceneNumber(button.value);
        break;
      case BUTTON_BLOCK_BYPASS:
        axeToggleBlock(button.value);
        break;
      case BUTTON_NONE:
      default:
        break;
    }
  }
}

void expPedalsCallback(PedalNumber pedalNumber, uint8_t pedalPosition) {
  LOG(SEV_INFO, "Exp pedal %d position change: %d", pedalNumber, pedalPosition);

  ExpressionPedal exp = expressionPedals[pedalNumber];
  if (exp.ccNumber) {
    int8_t value = 127 * (pedalPosition - exp.calibrationMin) / (exp.calibrationMax - exp.calibrationMin);
    value = value < CC_MIN_VALUE ? CC_MIN_VALUE : value > CC_MAX_VALUE ? CC_MAX_VALUE : value;
    axeSendCC(exp.ccNumber, (uint8_t) value);
  }
}

void updateIndication() {
  updateLeds();
  updateScreen();
}

// Main

int main(void) {
  initBjDevLib();

  LCDInit(LS_NONE);
  LcdHideCursor();

  expRegisterPedalChangePositionCallback(expPedalsCallback);

  axeInit(updateIndication);

  updateIndication();

  while (1) {
    buttonsCallback(getButtonLastEvent());
    expProcess();
    axeUpdate();
  }
}
