#include "axefx.h"

char* axeGetPresetName();

uint16_t axeGetPresetNumber();

uint8_t axeGetSceneNumber();

bool axeGetBlockActive(uint8_t blockId);

void axeSendCC(uint8_t ctrlNum, uint8_t val);

void axeSendPC(uint8_t progNum);

void axeSendFX(AxeFxFunctionId functionId);

void axeSetPresetNumber(uint16_t number);

void axeSetSceneNumber(uint8_t number);

void axeToggleBlock(uint8_t blockId);

void axeInit(void (*callback)());

void axeUpdate();
