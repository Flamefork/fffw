#include "bjdevlib_tb.h"
#include "axefx.h"

#include <string.h>

#include "common_defs.h"
#include "axe_state.h"

// State

static void (*updateCallback)();

char                  presetName[TB_LCD_WIDTH + 1] = VERSION_STRING;
uint16_t              presetNumber                 = 0xFFFF;
uint8_t               sceneNumber                  = 0xFF;
AxeFxEffectBlockState blockStates[0xFF];
AxeFxLooperInfo       looperState;

uint8_t looperCCs[] = {
  CC_LOOPER_RECORD,
  CC_LOOPER_PLAY,
  CC_LOOPER_ONCE,
  CC_LOOPER_DUB,
  CC_LOOPER_REV,
  CC_LOOPER_HALF,
  CC_LOOPER_UNDO,
};

char *axeGetPresetName() {
  return presetName;
}

uint16_t axeGetPresetNumber() {
  return presetNumber;
}

uint8_t axeGetSceneNumber() {
  return sceneNumber;
}

bool axeGetBlockActive(uint8_t blockId) {
  return blockStates[blockId].isEnabled_;
}

bool axeIsLooperState(uint8_t bit) {
  return (looperState.status & (1 << bit)) != 0;
}

// Actions

void axeSendCC(uint8_t ctrlNum, uint8_t val) {
  midiSendControlChange(ctrlNum, val, MY_AXEFX_MIDI_CHANNEL);
  LOG(SEV_INFO, "SENT CC: %d = %d", ctrlNum, val);
}

void axeSendPC(uint8_t progNum) {
  midiSendProgramChange(progNum, MY_AXEFX_MIDI_CHANNEL);
  LOG(SEV_INFO, "SENT PC: %d", progNum);
}

void axeSendFXPayload(AxeFxFunctionId functionId, uint8_t *payload, uint16_t payloadLength) {
  axefxSendFunctionRequest(MY_AXEFX_MODEL, functionId, payload, payloadLength);
  LOG(SEV_INFO, "SENT FX: %02X", functionId);
}

void axeSendFX(AxeFxFunctionId functionId) {
  axeSendFXPayload(functionId, NULL, 0);
}

void axeSetPresetNumber(uint16_t number) {
  axeSendCC(CC_BANK_CHANGE, number / 128);
  axeSendPC(number % 128);
}

void axeSetSceneNumber(uint8_t number) {
  axeSendCC(CC_SCENE_SELECT, number);
}

void axeToggleBlock(uint8_t blockId) {
  if (!blockStates[blockId].iaCcNumber_) {
    LOG(SEV_WARNING, "Unknown CC for block %d", blockId);
    return;
  }
  uint8_t ccNumber = blockStates[blockId].iaCcNumber_;
  uint8_t ccValue = blockStates[blockId].isEnabled_ ? CC_MIN_VALUE : CC_MAX_VALUE;
  axeSendCC(ccNumber, ccValue);

  axeSendFX(AXEFX_GET_PRESET_BLOCKS_FLAGS);
}

void axeEnableLooperStatus() {
  uint8_t payload = 1;
  axeSendFXPayload(MIDI_LOOPER_STATUS, &payload, 1);
}

void axeToggleLooperState(uint8_t bit) {
  uint8_t ccNumber = looperCCs[bit];
  uint8_t ccValue = axeIsLooperState(bit) ? CC_MIN_VALUE : CC_MAX_VALUE;
  axeSendCC(ccNumber, ccValue);
}

// State management

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

void sysExCallback(uint16_t length) {
  uint8_t *sysexData   = midiGetLastSysExData();

  if (!axeFxCheckFractalManufId(sysexData)) {
    return;
  }

  AxeFxFunctionId function = axeFxGetFunctionId(sysexData);
  LOG(SEV_INFO, "GOT FX: %02X", function);

  bool    shouldUpdate = false;

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

      axeSendFX(AXEFX_GET_PRESET_NAME);
      axeSendFX(AXEFX_GET_PRESET_BLOCKS_FLAGS);
      break;

    case AXEFX_SET_SCENE_NUMBER:
      sceneNumber  = axefxGetSceneNumber(sysexData);
      shouldUpdate = true;
      break;

    case AXEFX_FRONT_PANEL_CHANGE_DETECTED:
      axeSendFX(AXEFX_GET_PRESET_BLOCKS_FLAGS);
      break;

    case AXEFX_TEMPO_BEAT:
      break;

    case MIDI_LOOPER_STATUS:
      axefxParseLooperInfo(&looperState, sysexData);
      shouldUpdate = true;
      break;

    default:
      LOG(SEV_WARNING, "Unhandled FX: %02X", function);
      break;
  }

  if (shouldUpdate) {
    (*updateCallback)();
  }
}

// Lifecycle

void axeInit(void (*callback)()) {
  updateCallback = callback;
  midiRegisterSysExCallback(sysExCallback);
  axeSendFX(AXEFX_GET_PRESET_NUMBER);
  axeEnableLooperStatus();
}

void axeUpdate() {
  midiRead();
}
