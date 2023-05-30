#ifndef ARMV8_25_BITWISEINSTRUCTION_H
#define ARMV8_25_BITWISEINSTRUCTION_H

#include <stdint.h>

typedef struct CarryPair {
	int64_t shift;
	int64_t trunc;
}CarryPair;

CarryPair LogicalSL(int64_t* operand, int amount, int truncate);

CarryPair LogicalSR(int64_t* operand, int amount, int truncate);

CarryPair ArithmeticSR(int64_t* operand, int amount, int truncate);

CarryPair RotateRight(int64_t* operand, int amount, int truncate);

CarryPair ExecuteShift(int64_t shiftType, int *operand, int amount, int truncate);

#endif //ARMV8_25_BITWISEINSTRUCTION_H