#include "assembleSpecial.h"
#include <stdio.h>
#include <stdlib.h>

int32_t assembleSpecial(char **instruction, SPOperation op)
{
    switch (op) {
        case SPECIAL_NOP:
            return 0xd503201f;
        case SPECIAL_DOT_INT:
            return strtol(instruction[1], NULL, 0);
        case SPECIAL_AND_END:
            return 0x8a000000;
        default:
            perror("Invalid special instruction");
            exit(EXIT_FAILURE);
    }
}
