#ifndef ARMV8_25_ASSEMBLEUTILITY_H
#define ARMV8_25_ASSEMBLEUTILITY_H

#include <inttypes.h>

// Interval is [start, end)
char *substr(char *string, int start, int end);

// Interval is [1, strlen())
char *tail(char *string);

// splits instruction into words
char **split(char *instruction);

void setBits(int *instruction, int mask, int start);

// Truncate a 32-bit number to a maximum of bitCount (bits higher than that are set to 0).
uint32_t truncateBits(uint32_t inputs, int bitCount);

int getRegister(char *c);

int getImmediate(char *c);

#endif //ARMV8_25_ASSEMBLEUTILITY_H