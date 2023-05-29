#ifndef ARMV8_25_CONTROL_H
#define ARMV8_25_CONTROL_H

#include <stdint.h>

typedef enum  {IMMEDIATE, REGISTER, LOADSTORE, BRANCH, FIRST = IMMEDIATE, LAST = BRANCH, UNDEFINED} instructionType;

int64_t getBits(int start, int end, int64_t instruction);

uint64_t getBitsUnsigned(int start, int end, uint64_t instruction);

instructionType getInstructionType(int instruction);

#endif //ARMV8_25_CONTROL_H