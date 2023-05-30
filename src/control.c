#include "control.h"
#include "state.h"
#include "immediateInstruction.h"
#include "branchInstruction.h"
#include "registerInstruction.h"
#include "sdtInstruction.h"
#include "outputFileGenerator.h"
#include <stdio.h>
#include <stdlib.h>

bool getBit(int pos, int64_t instruction)
{
    return instruction & (1 << pos);
}

// Returns bits at the interval <start, end> (interval is closed) and does unsigned extend.
uint64_t getBits(int start, int end, int64_t instruction)
{
    return (instruction &  ((1ll << (end + 1)) - (1ll << start))) >> start;
}

int64_t getBitsSignExt(int start, int end, int64_t instruction)
{
    const int64_t mask = getBit(end, instruction) ? INT64_MAX : 0ll;
    return (mask << (end - start)) | getBits(start, end, instruction);
}

// Returns type of the instruction
instructionType getInstructionType(int instruction)
{
    int bitmasks[] = {0b1000, 0b0101, 0b0100, 0b1010};
    int dontCares[] = {0b1110, 0b0111, 0b0101, 0b1110};
    int opcode = getBits(25, 28, instruction);
    instructionType type = UNDEFINED;

    for (instructionType i = FIRST; i <= LAST; i++)
    {
        if ((dontCares[i] & opcode) == bitmasks[i])
        {
            type = i;
        }
    }
    return type;
}

bool ExecuteSpecialInstruction(int32_t instruction, ComputerState* computerState, char* outputFilePath)
{
    switch(instruction) {
        case 0xd503201f: // NOP
            computerState->PC += 4;
            break;
        case 0x8a000000: // Halt
            //Send to output file generator
            generateOutputFile(computerState, outputFilePath);
            exit(0);
            break;
        default:
            return false;
    }
    return true;
}

void ExecuteInstruction(int32_t instruction, ComputerState *computerState, char* outputFilePath)
{

    //Special instructions
    if(ExecuteSpecialInstruction(instruction, computerState, outputFilePath))
        return;

    //Normal instuctions
    switch (getInstructionType(instruction)) {
        case IMMEDIATE:
            ExecuteImmediate(instruction, computerState);
            break;
        case REGISTER:
            ExecuteRegister(instruction, computerState);
            break;
        case LOADSTORE:
            ExecuteSdtInstruction(instruction, computerState);
            break;
        case BRANCH:
            ExecuteBranch(instruction, computerState);
            break;
        default:
            perror("Unsolved type of instruction");
            exit(EXIT_FAILURE);
    }
}
