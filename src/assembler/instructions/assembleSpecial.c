#include "assembleSpecial.h"
#include <stdio.h>
#include <stdlib.h>

int32_t assembleSpecial(char **instruction, int type)
{
    switch (type) {
        case 0:
            return 0xd503201f;
        case 1:
            return strtol(instruction[1], NULL, 0);
        case 2:
            return 0x8a000000;
        default:
            printf("%d is not special. It does not deserve to be treated differently", type);
            exit(EXIT_FAILURE);
    }
}