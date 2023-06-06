#ifndef ARMV8_SDT_INSTRUCTION_H
#define ARMV8_SDT_INSTRUCTION_H
#include "../util/state.h"

void ExecuteSdtInstruction(const int64_t instruction, ComputerState *computerState);
#endif // ARMV8_SDT_INSTRUCTION_H

