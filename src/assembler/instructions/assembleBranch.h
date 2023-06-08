#ifndef ARMV8_25_ASSEMBLEBRANCH_H
#define ARMV8_25_ASSEMBLEBRANCH_H

#include <stdio.h>
#include <stdint.h>

typedef enum 
{
    B_UNCOND,
    B_REG,
    B_COND
} BOperation;

int32_t findCondition(char* bracnhCond);

int32_t assembleBranch(char** tokenized, BOperation op, int64_t PC);


#endif //ARMV8_25_ASSEMBLEBRANCH_H
