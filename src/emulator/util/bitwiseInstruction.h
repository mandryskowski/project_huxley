#ifndef ARMV8_25_BITWISEINSTRUCTION_H
#define ARMV8_25_BITWISEINSTRUCTION_H

#include <stdint.h>

void LogicalSL(int64_t* operand, int amount, int truncate);

void LogicalSR(int64_t* operand, int amount, int truncate);

void ArithmeticSR(int64_t* operand, int amount, int truncate);

void RotateRight(int64_t* operand, int amount, int truncate);

void ExecuteShift(int shiftType, int64_t *operand, int amount, int truncate);

#endif //ARMV8_25_BITWISEINSTRUCTION_H
