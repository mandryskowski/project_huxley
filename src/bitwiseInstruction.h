#ifdef ARMV8_25_BITWISEINSTRUCTION_H
#define ARMV8_25_BITWISEINSTRUCTION_H

#include "state.h"

int64_t LogicalSL(int* operand, int amount, int mode)

int64_t LogicalSR(int* operand, int amount, int mode)

int64_t ArithmeticSR(int* operand, int amount, int mode)

void RotateRight(int* operand, int amount, int mode)

#endif //ARMV8_25_BITWISEINSTRUCTION_H
