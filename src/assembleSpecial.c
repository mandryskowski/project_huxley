#include "assembleSpecial.h"
#include <stdio.h>
#include <stdlib.h>

int32_t assembleSpecial(char **instruction, SPOperation op)
{
    switch (type) {
        case NOP:
            return 0xd503201f;
        case DOT_INT:
            return strtol(instruction[1], NULL, 0);
        case AND_END:
            return 0x8a000000;
        default:
            exit(EXIT_FAILURE);
    }
}