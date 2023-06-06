#ifndef ARMV8_25_BITWISEINSTRUCTION_H
#define ARMV8_25_BITWISEINSTRUCTION_H

#include <stdint.h>
#include <stdbool.h>

int64_t LogicalSL(int64_t operand, int amount, bool is64bit);

int64_t LogicalSR(int64_t operand, int amount, bool is64bit);

int64_t ArithmeticSR(int64_t operand, int amount, bool is64bit);

int64_t RotateRight(int64_t operand, int amount, bool is64bit);

int64_t ExecuteShift(int shiftType, int64_t operand, int amount, bool is64bit);

#endif //ARMV8_25_BITWISEINSTRUCTION_H

