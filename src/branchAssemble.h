#ifndef ARMV8_25_BRANCHASSEMBLE_H
#define ARMV8_25_BRANCHASSEMBLE_H

#include <stdio.h>
#include <stdint.h>
#include "label.h"

int32_t getSimm(char *c, Label* labels);
int32_t branchOpcode (char *c, Label* labels);

#endif //ARMV8_25_BRANCHASSEMBLE_H
