#ifndef ARMV8_25_EMULATECONTROL_H
#define ARMV8_25_EMULATECONTROL_H
#include <stdint.h>
#include "util/state.h"

bool ExecuteInstruction(int32_t instruction, ComputerState*, char* outputFilePath);

#endif //ARMV8_25_EMULATECONTROL_H
