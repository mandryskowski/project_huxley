#ifndef ARMV8_25_EMULATEUTILITY_H
#define ARMV8_25_EMULATEUTILITY_H

#include <inttypes.h>
#include <stdbool.h>

bool getBit(int pos, int64_t instruction);
uint64_t getBits(int start, int end, int64_t instruction);
int64_t getBitsSignExt(int start, int end, int64_t instruction);

#endif //ARMV8_25_EMULATEUTILITY_H