#ifndef ARMV8_25_ASSEMBLESPECIAL_H
#define ARMV8_25_ASSEMBLESPECIAL_H

#include <stdint.h>

typedef enum {
    SPECIAL_NOP,
    SPECIAL_DIRECTIVE,
    SPECIAL_HOLD
} SPECIALOperation;

int32_t assembleSpecial(char **instruction, int type);

#endif //ARMV8_25_ASSEMBLESPECIAL_H
