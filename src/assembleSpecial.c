#include "assembleSpecial.h"
#include <stdio.h>
#include <stdlib.h>

/* Assembles special operation*/
int32_t assembleSpecial(char **instruction, int type)
{
    switch (type) {
        case SPECIAL_NOP:
            return 0xd503201f;
        case SPECIAL_DIRECTIVE:
            return strtol(instruction[1], NULL, 0);
        case SPECIAL_HOLD:
            return 0x8a000000;
        default:
            exit(EXIT_FAILURE);
    }
}