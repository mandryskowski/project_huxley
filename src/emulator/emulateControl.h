#ifndef ARMV8_25_EMULATECONTROL_H
#define ARMV8_25_EMULATECONTROL_H
#include <stdint.h>
#include "util/state.h"

void ExecuteInstruction(int32_t instruction, ComputerState *computerState, char*);

#endif //ARMV8_25_EMULATECONTROL_H
