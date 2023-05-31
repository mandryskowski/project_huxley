#ifndef ARMV8_25_IMMEDIATEINSTRUCTION_H
#define ARMV8_25_IMMEDIATEINSTRUCTION_H

#include "state.h"

void ExecuteImmediate(int instruction, ComputerState * computerState);

void runAddition(bool sf, int opc, int rd, int64_t op, int64_t registerValue, ComputerState *computerState);

#endif //ARMV8_25_IMMEDIATEINSTRUCTION_H
