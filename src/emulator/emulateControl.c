#include <stdio.h>
#include <stdlib.h>
#include "emulateControl.h"
#include "util/outputFileGenerator.h"
#include "util/emulateUtility.h"
#include "instructions/immediateInstruction.h"
#include "instructions/branchInstruction.h"
#include "instructions/registerInstruction.h"
#include "instructions/sdtInstruction.h"

typedef enum  {IMMEDIATE, REGISTER, LOADSTORE, BRANCH, FIRST = IMMEDIATE, LAST = BRANCH, UNDEFINED} instructionType;

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
