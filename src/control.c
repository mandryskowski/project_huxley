#include "control.h"
#include <stdio.h>


int getBits(int start, int end, int instruction){
    return (instruction &  ((1 << (start + 1)) - (1 << end))) >> end;
}

instructionType getInstruction(int instruction){
    int bitmasks[] = {0b1000, 0b0101, 0b0100, 0b1010};
    int dontCares[] = {0b1110, 0b0111, 0b0101, 0b1110};
    int opcode = getBits(28, 25, instruction);
    instructionType type = UNDEFINED;

    for (instructionType i = FIRST; i <= LAST; i++){
        if ((dontCares[i] & opcode) == bitmasks[i]){
            type = i;
        }
    }
    return type;
}

void ExecuteInstruction(int instruction, instructionType type){
    switch (type) {
        case IMMEDIATE:
            //ExecuteImmediate(instruction);
            break;
        case REGISTER:
            //ExecuteRegister(instruction);
            break;
        case LOADSTORE:
            //ExecuteLoadStore(instruction);
            break;
        case BRANCH:
            //ExecuteBranch(instruction);
            break;
        default:
            perror("Incorrect opcode");
        }
}

