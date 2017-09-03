#include "bjdevlib_tb.h"
#include "lcd_tb.h"

#include "axe_state.h"
#include "settings.h"

// State

ButtonPage *page;
bool isMetronomeActive;

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
      case BUTTON_TAP_TEMPO:
        isActive = axeIsTempoBeat();
        break;
      case BUTTON_PAGE:
        isActive = true;
        break;
      default:
        break;
    }

    if (i != PEDAL_LED) {
      LedColor color = isActive ? button.color : COLOR_BLACK;
      ledSetColor(i, color, false);
    } else {
      PedalLedColor color = isActive ? button.pedalColor : PEDAL_COLOR_NO;
      ledSetPedalColorAll(PEDAL_COLOR_NO, false);
      ledSetPedalColor(PEDAL_LED_NUM(-7), color, false);
      ledSetPedalColor(PEDAL_LED_NUM(+7), color, false);
    }
  }

  AxeTunerState *tunerState = axeGetTunerState();
  if (tunerState->isEnabled) {
    int8_t dev = tunerState->deviation;
    if (ABS(dev) <= 1) {
      ledSetPedalColor(PEDAL_LED_NUM(-1), PEDAL_COLOR_G, false);
      ledSetPedalColor(PEDAL_LED_NUM(+1), PEDAL_COLOR_G, false);
    } else if (ABS(dev) <= 2) {
      ledSetPedalColor(PEDAL_LED_NUM(SIGN(dev)), PEDAL_COLOR_G, false);
    } else if (ABS(dev) <= 4) {
      ledSetPedalColor(PEDAL_LED_NUM(SIGN(dev)), PEDAL_COLOR_RG, false);
    } else {
      uint8_t height = 1 + 4 * ABS(dev) / AXEFX_TUNER_MAX_ABS_DEVIATION;
      if (height > 5) {
        height = 5;
      }
      for (int8_t i = 1; i <= height; i++) {
        ledSetPedalColor(PEDAL_LED_NUM(SIGN(dev) * i), PEDAL_COLOR_R, false);
      }
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

  AxeTunerState *tunerState = axeGetTunerState();
  if (tunerState->isEnabled) {
    LCDWriteStringXY(TB_LCD_WIDTH - 4, 1, "    ");
    LCDWriteStringXY(TB_LCD_WIDTH - 3, 1, tunerState->note);
    LCDWriteIntXY(TB_LCD_WIDTH - 1, 1, tunerState->stringNumber, 1);
  }
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
      case BUTTON_TAP_TEMPO:
        axeSendCC(CC_TEMPO, (uint8_t) CC_MAX_VALUE);
        break;
      case BUTTON_PAGE:
        page = &pages[button.value];
        updateIndication();
        break;
      default:
        break;
    }
  }

  if (buttonEvent.actionType_ == BUTTON_HOLDON) {
    Button button = page->buttons[buttonEvent.buttonNum_];
    switch (button.type) {
      case BUTTON_TAP_TEMPO:
        isMetronomeActive = !isMetronomeActive;
        uint8_t ccValue = isMetronomeActive ? CC_MAX_VALUE : CC_MIN_VALUE;
        axeSendCC(CC_METRONOME, ccValue);
        break;
      default:
        break;
    }
  }
}

void expPedalsCallback(PedalNumber pedalNumber, uint8_t pedalPosition) {
  LOG(SEV_INFO, "Exp pedal %d position change: %d", pedalNumber, pedalPosition);

  ExpressionPedal exp = expressionPedals[pedalNumber];
  int8_t value = 127 * (pedalPosition - exp.calibrationMin) / (exp.calibrationMax - exp.calibrationMin);
  value = value < CC_MIN_VALUE ? CC_MIN_VALUE : value > CC_MAX_VALUE ? CC_MAX_VALUE : value;
  if (exp.ccNumber) {
    axeSendCC(exp.ccNumber, (uint8_t) value);
  }
  if (exp.toggleTunerBelow) {
    axeToggleTuner(value < exp.toggleTunerBelow);
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
