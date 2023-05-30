#ifndef ARMV8_25_BITWISEINSTRUCTION_H
#define ARMV8_25_BITWISEINSTRUCTION_H

typedef struct CarryPair {
	int64_t shift;
	int64_t trunc;
}CarryPair;

CarryPair LogicalSL(int* operand, int amount, int truncate);

CarryPair LogicalSR(int* operand, int amount, int truncate);

CarryPair ArithmeticSR(int* operand, int amount, int truncate);

CarryPair RotateRight(int* operand, int amount, int truncate);

CarryPair ExecuteShift(int shiftType, int *operand, int amount, int truncate);

#endif //ARMV8_25_BITWISEINSTRUCTION_H