#include "control.h"
#include "state.h"
#include <stdio.h>

// Returns bits at the interval <start, end> (interval is closed).
int getBits(int end, int start, int instruction){
    return (instruction &  ((1 << (end + 1)) - (1 << start))) >> end;
}

// Returns type of the instruction
instructionType getInstructionType(int instruction){
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

void ExecuteInstruction(int instruction, ComputerState *computerState){
    switch (getInstructionType(instruction)) {
        case IMMEDIATE:
            //ExecuteImmediate(instruction, computerState);
            break;
        case REGISTER:
            //ExecuteRegister(instruction, computerState);
            break;
        case LOADSTORE:
            //ExecuteLoadStore(instruction, computerState);
            break;
        case BRANCH:
            //ExecuteBranch(instruction, computerState);
            break;
        default:
            perror("Incorrect opcode");
        }
}

