#ifndef ARMV8_25_ASSEMBLESPECIAL_H
#define ARMV8_25_ASSEMBLESPECIAL_H

#include <stdint.h>

typedef enum {
    NOP,
    DOT_INT,
    AND_END
} SPOperation;

int32_t assembleSpecial(char **instruction, SPOperation op);

#endif //ARMV8_25_ASSEMBLESPECIAL_H
