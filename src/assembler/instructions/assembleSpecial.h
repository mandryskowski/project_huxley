#ifndef ARMV8_25_ASSEMBLESPECIAL_H
#define ARMV8_25_ASSEMBLESPECIAL_H

#include <stdint.h>

typedef enum {
    SPECIAL_NOP,
    SPECIAL_DOT_INT,
    SPECIAL_AND_END
} SPOperation;

int32_t assembleSpecial(char **instruction, SPOperation op);

#endif //ARMV8_25_ASSEMBLESPECIAL_H
