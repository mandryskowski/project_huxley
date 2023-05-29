#include "control.h"
#include "state.h"
#include "immediateInstruction.h"
#include <stdio.h>
#include <stdlib.h>

// Returns bits at the interval <start, end> (interval is closed).
int64_t getBits(int start, int end, int64_t instruction)
{
    return (instruction &  ((1ll << (end + 1)) - (1ll << start))) >> start;
}

uint64_t getBitsUnsigned(int start, int end, uint64_t instruction)
{
    return (instruction &  ((1ull << (end + 1)) - (1ull << start))) >> start;
}

// Returns type of the instruction
instructionType getInstructionType(int instruction)
{
    int bitmasks[] = {0b1000, 0b0101, 0b0100, 0b1010};
    int dontCares[] = {0b1110, 0b0111, 0b0101, 0b1110};
    int opcode = getBits(25, 28, instruction);
    instructionType type = UNDEFINED;

    for (instructionType i = FIRST; i <= LAST; i++){
        if ((dontCares[i] & opcode) == bitmasks[i]){
            type = i;
        }
    }
    return type;
}

void ExecuteInstruction(int instruction, ComputerState *computerState)
{
    instructionType type = getInstructionType(instruction);

    switch (type)
    {
        case IMMEDIATE:
            ExecuteImmediate(instruction, computerState);
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
            fprintf(stderr, "Instruction type: %d is not handled by any function\n", type);
            exit(EXIT_FAILURE);
    }
}

