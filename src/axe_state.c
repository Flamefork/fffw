#include "bjdevlib_tb.h"
#include "axefx.h"

#include <string.h>

#include "common_defs.h"
#include "axe_state.h"

#define SPECULATIVE_UPDATES true

// State

bool shouldUpdate;
void (*updateCallback)();

char presetName[TB_LCD_WIDTH + 1] = VERSION_STRING;
uint16_t presetNumber = 0xFFFF;
uint8_t sceneNumber;
AxeFxEffectBlockState blockStates[AXEFX_MAX_BLOCK_ID - AXEFX_MIN_BLOCK_ID];
AxeFxLooperInfo looperState;
AxeTunerState tunerState;
bool isTempoBeat = false;

uint8_t looperCCs[] = {
    CC_LOOPER_RECORD,
    CC_LOOPER_PLAY,
    CC_LOOPER_ONCE,
    CC_LOOPER_DUB,
    CC_LOOPER_REV,
    CC_LOOPER_HALF,
    CC_LOOPER_UNDO,
};

char *noteNames[12] = {
    "A ",
    "A#",
    "B ",
    "C ",
    "C#",
    "D ",
    "D#",
    "E ",
    "F ",
    "F#",
    "G ",
    "G#",
};

AxeFxMultipurposeResponseInfo responseInfo;

uint32_t tunerLastSeen = 0;
uint32_t tempoLastSeen = 0;

#define TUNER_TIMEOUT 5
#define TEMPO_TIMEOUT 2

// State getters

char *axeGetPresetName() {
  return presetName;
}

uint16_t axeGetPresetNumber() {
  return presetNumber;
}

uint8_t axeGetSceneNumber() {
  return sceneNumber;
}

bool axeIsBlockActive(uint8_t blockId) {
  return blockStates[blockId - AXEFX_MIN_BLOCK_ID].isEnabled_;
}

bool axeIsBlockOnX(uint8_t blockId) {
  return blockStates[blockId - AXEFX_MIN_BLOCK_ID].isX_;
}

bool axeIsBlockAvailable(uint8_t blockId) {
  return blockStates[blockId - AXEFX_MIN_BLOCK_ID].effectId_ != 0;
}

bool axeIsLooperState(uint8_t bit) {
  return (looperState.status & (1 << bit)) != 0;
}

uint8_t axeGetLooperPosition() {
  return looperState.position;
}

AxeTunerState *axeGetTunerState() {
  return &tunerState;
}

bool axeIsTempoBeat() {
  return isTempoBeat;
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
  if (SPECULATIVE_UPDATES) {
    presetNumber = number;
  }
}

void axeSetSceneNumber(uint8_t number) {
  axeSendCC(CC_SCENE_SELECT, number - 1);
  if (SPECULATIVE_UPDATES) {
    sceneNumber = number;
  }
}

void axeToggleBlock(uint8_t blockId) {
  AxeFxEffectBlockState blockState = blockStates[blockId - AXEFX_MIN_BLOCK_ID];
  uint8_t ccNumber = blockState.iaBypassCcNumber_;
  uint8_t ccValue = blockState.isEnabled_ ? CC_MIN_VALUE : CC_MAX_VALUE;

  if (!ccNumber || ccNumber > 0x7F) {
    LOG(SEV_WARNING, "Unknown Bypass CC for block %d", blockId);
    return;
  }
  axeSendCC(ccNumber, ccValue);

  axeSendFX(AXEFX_PRESET_BLOCKS_DATA);
  if (SPECULATIVE_UPDATES) {
    blockStates[blockId - AXEFX_MIN_BLOCK_ID].isEnabled_ = !blockState.isEnabled_;
  }
}

void axeToggleBlockXY(uint8_t blockId) {
  AxeFxEffectBlockState blockState = blockStates[blockId - AXEFX_MIN_BLOCK_ID];
  uint8_t ccNumber = blockState.iaXYCcNumber_;
  uint8_t ccValue = blockState.isX_ ? CC_MIN_VALUE : CC_MAX_VALUE;

  if (!ccNumber || ccNumber > 0x7F) {
    LOG(SEV_WARNING, "Unknown XY CC for block %d", blockId);
    return;
  }
  axeSendCC(ccNumber, ccValue);

  axeSendFX(AXEFX_PRESET_BLOCKS_DATA);
  if (SPECULATIVE_UPDATES) {
    blockStates[blockId - AXEFX_MIN_BLOCK_ID].isX_ = !blockState.isX_;
  }
}

void axeToggleLooperListener(bool enable) {
  uint8_t payload = enable ? 1 : 0;
  axeSendFXPayload(AXEFX_LOOPER_STATUS, &payload, 1);
}

void axeToggleLooperState(uint8_t bit) {
  uint8_t ccNumber = looperCCs[bit];
  uint8_t ccValue = axeIsLooperState(bit) ? CC_MIN_VALUE : CC_MAX_VALUE;
  axeSendCC(ccNumber, ccValue);
  if (SPECULATIVE_UPDATES) {
    looperState.status = axeIsLooperState(bit) ? looperState.status - (1 << bit) : looperState.status | (1 << bit);
  }
}

void axeToggleTuner(bool enable) {
  axeSendCC(CC_TUNER, enable ? CC_MAX_VALUE : CC_MIN_VALUE);
}

// State management

void parseBlockInfo(uint8_t *sysexData) {
  uint8_t totalEffectsInMessage = axefxGetEffectBlockStateNumber(sysexData);

  memset(&blockStates, 0, sizeof blockStates);
  for (uint8_t i = 0; i < totalEffectsInMessage; ++i) {
    AxeFxEffectBlockState state;
    if (axefxGetSingleEffectBlockState(&state, i, sysexData)) {
      blockStates[state.effectId_ - AXEFX_MIN_BLOCK_ID] = state;
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

  switch (function) {
    case AXEFX_PRESET_BLOCKS_DATA:
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
      axeSendFX(AXEFX_PRESET_BLOCKS_DATA);
      break;

    case AXEFX_SET_SCENE_NUMBER:
      sceneNumber = axefxGetSceneNumber(sysexData) + 1;
      shouldUpdate = true;
      break;

    case AXEFX_FRONT_PANEL_CHANGE_DETECTED:
      axeSendFX(AXEFX_PRESET_BLOCKS_DATA);
      break;

    case AXEFX_TEMPO_BEAT:
      isTempoBeat = true;
      tempoLastSeen = getTicks();
      shouldUpdate = true;
      break;

    case AXEFX_LOOPER_STATUS:
      axefxParseLooperInfo(&looperState, sysexData);
      shouldUpdate = true;
      break;

    case AXEFX_TUNER_INFO:
      tunerLastSeen = getTicks();
      AxeFxEffectTunerInfo tunerInfo;
      axefxParseTunerInfo(&tunerInfo, sysexData);
      tunerState.isEnabled = true;
      tunerState.note = noteNames[tunerInfo.note_];
      tunerState.stringNumber = tunerInfo.stringNumber_ + 1;
      tunerState.deviation = tunerInfo.tuneData_ - 63;
      shouldUpdate = true;
      break;

    case AXEFX_MULTIPURPOSE_RESPONSE:
      axefxParseMultipurposeResponseInfo(&responseInfo, sysexData);
      LOG(SEV_INFO, "Multipurpose response FX: %02X, code: %02X", responseInfo.functionId, responseInfo.code);
      break;

    default:
      LOG(SEV_WARNING, "Unhandled FX: %02X", function);
      break;
  }
}

void checkTunerTimeout() {
  if (tunerState.isEnabled && getTicks() - tunerLastSeen > TUNER_TIMEOUT) {
    tunerState.isEnabled = false;
    shouldUpdate = true;
  }
}

void checkTempoTimeout() {
  if (isTempoBeat && getTicks() - tempoLastSeen > TEMPO_TIMEOUT) {
    isTempoBeat = false;
    shouldUpdate = true;
  }
}

void programChangeCallback(uint8_t channel, uint8_t program) {
  LOG(SEV_INFO, "GOT PC: %d", program);
}

void controlChangeCallback(uint8_t channel, uint8_t ccNum, uint8_t ccVal) {
  LOG(SEV_INFO, "GOT CC: %d = %d", ccNum, ccVal);
}


// Lifecycle

void axeInit(void (*callback)()) {
  updateCallback = callback;
  midiRegisterSysExCallback(sysExCallback);
  midiRegisterProgramChangeCallback(programChangeCallback);
  midiRegisterControlChangeCallback(controlChangeCallback);
  axeSendFX(AXEFX_GET_PRESET_NUMBER);
}

void axeUpdate() {
  shouldUpdate = false;
  midiRead();
  checkTunerTimeout();
  checkTempoTimeout();

  if (shouldUpdate) {
    (*updateCallback)();
  }
}
