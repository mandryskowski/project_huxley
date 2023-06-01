#ifndef ARMV8_25_BRANCHASSEMBLE_H
#define ARMV8_25_BRANCHASSEMBLE_H

#include <stdio.h>
#include <stdint.h>
#include "label.h"

typedef enum 
{
    B_UNCOND,
    B_REG,
    B_COND
} BOperation;


int32_t getSimm(char* c, Label* labels);
int32_t branchOpcode (char** tokenized, Label* labels, BOperation op);

#endif //ARMV8_25_BRANCHASSEMBLE_H
