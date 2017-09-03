#include "axefx.h"

typedef struct AxeTunerState {
  bool isEnabled;
  char *note;
  uint8_t stringNumber;
  int8_t deviation;
} AxeTunerState;

char *axeGetPresetName();

uint16_t axeGetPresetNumber();

uint8_t axeGetSceneNumber();

bool axeGetBlockActive(uint8_t blockId);

bool axeIsLooperState(uint8_t bit);

AxeTunerState *axeGetTunerState();

bool axeIsTempoBeat();

void axeToggleLooperState(uint8_t bit);

void axeToggleTuner(bool enable);

void axeSendCC(uint8_t ctrlNum, uint8_t val);

void axeSendPC(uint8_t progNum);

void axeSendFXPayload(AxeFxFunctionId functionId, uint8_t *payload, uint16_t payloadLength);

void axeSendFX(AxeFxFunctionId functionId);

void axeSetPresetNumber(uint16_t number);

void axeSetSceneNumber(uint8_t number);

void axeToggleBlock(uint8_t blockId);

void axeInit(void (*callback)());

void axeUpdate();
