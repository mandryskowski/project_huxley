#ifndef ARMV8_25_BRANCHASSEMBLE_H
#define ARMV8_25_BRANCHASSEMBLE_H

#include <stdio.h>
#include <stdint.h>

int32_t getSimm(char *c);
int32_t branchOpcode (char *c);

#endif //ARMV8_25_BRANCHASSEMBLE_H
