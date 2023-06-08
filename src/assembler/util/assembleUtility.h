#ifndef ARMV8_25_ASSEMBLEUTILITY_H
#define ARMV8_25_ASSEMBLEUTILITY_H

#include <inttypes.h>

void free2DArray(void **ptr);

/* Finds index of an element in the list equal to (char *element).
 * if the element is not found returns -1.*/
int find(char **list, char *element);

/* If a instruction is an alias it is replaced by its meaning.
 * Otherwise, it returns the given instruction itself.*/
void getAlias(char **instruction);

// Interval is [start, end)
char *substr(char *string, int start, int end);

// Interval is [1, strlen())
char *tail(char *string);

// splits instruction into words
char **split(char *instruction);

void setBits(int *instruction, int mask, int start);

int getShiftCode(char *shiftID);

// Truncate a 32-bit number to a maximum of bitCount (bits higher than that are set to 0).
uint32_t truncateBits(uint32_t inputs, int bitCount);

int getRegister(char *c);

int getImmediate(char *c);

#endif //ARMV8_25_ASSEMBLEUTILITY_H
