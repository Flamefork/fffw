#include "bjdevlib_tb.h"
#include "lcd_tb.h"

#include "axe_state.h"
#include "settings.h"

// State

ButtonPage *page;
bool isMetronomeActive;
bool hasLooperButtons;

// Indication

void updateLeds() {
  for (uint8_t i = 0; i < FOOT_BUTTONS_NUM; i++) {
    bool isActive = false;
    bool isAlternated = false;
    Button button = page->buttons[i];
    switch (button.type) {
      case BUTTON_PRESET:
        isActive = button.value == axeGetPresetNumber();
        isAlternated = button.altValue == axeGetPresetNumber();
        break;
      case BUTTON_SCENE:
        isActive = button.value == axeGetSceneNumber();
        isAlternated = button.altValue == axeGetSceneNumber();
        break;
      case BUTTON_BLOCK_BYPASS:
        isActive = axeIsBlockActive(button.value);
        isAlternated = axeIsBlockAvailable(button.value);
        break;
      case BUTTON_BLOCK_XY:
        isActive = axeIsBlockActive(button.value) && axeIsBlockOnX(button.value);
        isAlternated = axeIsBlockActive(button.value) && !axeIsBlockOnX(button.value);
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
      LedColor color = isActive ? button.color : isAlternated ? button.altColor : COLOR_BLACK;
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
  LCDWriteStringXY(0, 0, page->name);
  LCDWriteStringXY(0, 1, axeGetPresetName());

  AxeTunerState *tunerState = axeGetTunerState();
  if (tunerState->isEnabled) {
    LCDWriteStringXY(TB_LCD_WIDTH - 4, 0, " ");
    LCDWriteStringXY(TB_LCD_WIDTH - 3, 0, tunerState->note);
    LCDWriteIntXY(TB_LCD_WIDTH - 1, 0, tunerState->stringNumber, 1);
  }

  if (hasLooperButtons && !axeIsBlockAvailable(AXEFX_BLOCK_LOOPER)) {
    LCDWriteStringXY(0, 0, "LOOPER NOT FOUND");
  }
  if (hasLooperButtons && axeIsLooperState(AXEFX_LOOPER_BIT_PLAY)) {
    uint8_t position = 7 + 9 * axeGetLooperPosition() / 100;
    char *marker = axeIsLooperState(AXEFX_LOOPER_BIT_REVERSE) ? "<" : ">";
    LCDWriteStringXY(position, 0, marker);
  }
}

void updateIndication() {
  updateLeds();
  updateScreen();
}

void updateLooperListener() {
  hasLooperButtons = false;
  for (uint8_t i = 0; i < FOOT_BUTTONS_NUM; i++) {
    if (page->buttons[i].type == BUTTON_LOOPER) {
      hasLooperButtons = true;
      break;
    }
  }
  axeToggleLooperListener(hasLooperButtons);
}

// Callbacks

void buttonsCallback(ButtonEvent buttonEvent) {
  if (buttonEvent.actionType_ == BUTTON_NO_EVENT) {
    return;
  }
  LOG(SEV_INFO, "Button %d event: %d", buttonEvent.buttonNum_, buttonEvent.actionType_);

  Button button = page->buttons[buttonEvent.buttonNum_];

  if (buttonEvent.actionType_ == BUTTON_PUSH) {
    switch (button.type) {
      case BUTTON_PRESET:
        axeSetPresetNumber(axeGetPresetNumber() == button.value ? button.altValue : button.value);
        break;
      case BUTTON_SCENE:
        axeSetSceneNumber(axeGetSceneNumber() == button.value ? button.altValue : button.value);
        break;
      case BUTTON_BLOCK_BYPASS:
        axeToggleBlock(button.value);
        break;
      case BUTTON_BLOCK_XY:
        axeToggleBlockXY(button.value);
        break;
      case BUTTON_LOOPER:
        axeToggleLooperState(button.value);
        break;
      case BUTTON_TAP_TEMPO:
        axeSendCC(CC_TEMPO, CC_MAX_VALUE);
        break;
      default:
        break;
    }
  }

  if (buttonEvent.actionType_ == BUTTON_RELEASE && button.momentary) {
    switch (button.type) {
      case BUTTON_BLOCK_BYPASS:
        axeToggleBlock(button.value);
        break;
      case BUTTON_BLOCK_XY:
        axeToggleBlockXY(button.value);
        break;
      default:
        break;
    }
  }

  if (buttonEvent.actionType_ == BUTTON_RELEASE && buttonEvent.previousActionType_ == BUTTON_PUSH) {
    switch (button.type) {
      case BUTTON_PAGE:
        page = &pages[button.value];
        updateLooperListener();
        break;
      default:
        break;
    }
  }

  if (buttonEvent.actionType_ == BUTTON_HOLDON) {
    switch (button.type) {
      case BUTTON_TAP_TEMPO:
        isMetronomeActive = !isMetronomeActive;
        uint8_t ccValue = isMetronomeActive ? CC_MAX_VALUE : CC_MIN_VALUE;
        axeSendCC(CC_METRONOME, ccValue);
        break;
      case BUTTON_PAGE:
        page = &pages[button.altValue];
        updateLooperListener();
        break;
      default:
        break;
    }
  }

  updateIndication();
}

void expPedalsCallback(PedalNumber pedalNumber, uint8_t pedalPosition) {
  LOG(SEV_INFO, "Exp pedal %d position change: %d", pedalNumber, pedalPosition);

  ExpressionPedal exp = expressionPedals[pedalNumber];
  pedalPosition = LIMIT(pedalPosition, exp.calibrationMin, exp.calibrationMax);
  uint8_t value = 127 * (pedalPosition - exp.calibrationMin) / (exp.calibrationMax - exp.calibrationMin);
  if (exp.ccNumber) {
    axeSendCC(exp.ccNumber, value);
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

  updateLooperListener();

  while (1) {
    buttonsCallback(getButtonLastEvent());
    expProcess();
    axeUpdate();
  }
}
