#ifndef ARMV8_25_CONTROL_H
#define ARMV8_25_CONTROL_H

#include <stdint.h>
#include <stdbool.h>
#include "control.h"
#include "state.h"
#include "immediateInstruction.h"
#include "outputFileGenerator.h"

typedef enum  {IMMEDIATE, REGISTER, LOADSTORE, BRANCH, FIRST = IMMEDIATE, LAST = BRANCH, UNDEFINED} instructionType;

bool getBit(int pos, int64_t instruction);
uint64_t getBits(int start, int end, int64_t instruction);
int64_t getBitsSignExt(int start, int end, int64_t instruction);

instructionType getInstructionType(int instruction);

void ExecuteInstruction(int32_t instruction, ComputerState *computerState);

#endif //ARMV8_25_CONTROL_H
