#include "bjdevlib_tb.h"
#include "lcd_tb.h"

#include "axe_state.h"
#include "settings.h"

// State

ButtonPage *page;

// Indication

void updateLeds() {
  for (uint8_t i = 0; i < FOOT_BUTTONS_NUM; i++) {
    bool isActive = false;
    Button button = page->buttons[i];
    switch (button.type) {
      case BUTTON_PRESET:
        isActive = button.value == axeGetPresetNumber();
        break;
      case BUTTON_SCENE:
        isActive = button.value == axeGetSceneNumber();
        break;
      case BUTTON_BLOCK_BYPASS:
        isActive = axeGetBlockActive(button.value);
        break;
      case BUTTON_LOOPER:
        isActive = axeIsLooperState(button.value);
        break;
      case BUTTON_PAGE:
        isActive = true;
        break;
      case BUTTON_NONE:
      default:
        break;
    }

    if (i != PEDAL_LED) {
      LedColor color = isActive ? button.color : COLOR_BLACK;
      ledSetColor(i, color, false);
    } else {
      PedalLedColor color = isActive ? button.pedalColor : PEDAL_COLOR_NO;
      ledSetPedalColorAll(PEDAL_COLOR_NO, false);
      ledSetPedalColor(1, color, false);
      ledSetPedalColor(9, color, false);
    }
  }

  ledSend();
  ledPedalSend();
}

void updateScreen() {
  LCDWriteStringXY(0, 0, "                ");
  LCDWriteStringXY(0, 0, page->name);
  LCDWriteStringXY(0, 1, "                ");
  LCDWriteStringXY(0, 1, axeGetPresetName());
}

void updateIndication() {
  updateLeds();
  updateScreen();
}

// Callbacks

void buttonsCallback(ButtonEvent buttonEvent) {
  if (buttonEvent.actionType_ == BUTTON_NO_EVENT) {
    return;
  }
  LOG(SEV_INFO, "Button %d event: %d", buttonEvent.buttonNum_, buttonEvent.actionType_);

  if (buttonEvent.actionType_ == BUTTON_PUSH) {
    Button button = page->buttons[buttonEvent.buttonNum_];
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
      case BUTTON_LOOPER:
        axeToggleLooperState(button.value);
        break;
      case BUTTON_PAGE:
        page = &pages[button.value];
        updateIndication();
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

// Main

int main(void) {
  initBjDevLib();

  LCDInit(LS_NONE);
  LcdHideCursor();

  page = &pages[0];

  expRegisterPedalChangePositionCallback(expPedalsCallback);

  axeInit(updateIndication);

  updateIndication();

  while (1) {
    buttonsCallback(getButtonLastEvent());
    expProcess();
    axeUpdate();
  }
}
