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
    /* To get the instruction type we test bits 25-28 against respective bitmasks.
    * We must also show which bits we don't care about (represented as X in the spec)
    *  to match all possible patterns. */

    // Bitmasks respectively: 0b1000, 0b0101, 0b0100, 0b1010
    int bitmasks[] = {0x8, 0x5, 0x4, 0xA};
    // Used bits respectively: 0b1110, 0b0111, 0b0101, 0b1110. The zero bits represent don't cares.
    int usedBits[] = {0xE, 0x7, 0x5, 0xE};

    int opcode = getBits(25, 28, instruction);
    instructionType type = UNDEFINED;

    for (instructionType i = FIRST; i <= LAST; i++)
    {
        if ((usedBits[i] & opcode) == bitmasks[i])
        {
            type = i;
        }
    }
    return type;
}

int ExecuteSpecialInstruction(int32_t instruction, ComputerState* computerState, char* outputFilePath)
{
    switch(instruction) {
        case 0xd503201f: // NOP
	    return 1;
            break;
        case 0x8a000000: // Halt
	    return 2; //Exit code
            break;
        default:
            return 0;
    }
}

bool ExecuteInstruction(int32_t instruction, ComputerState *computerState, char* outputFilePath)
{

    //Special instructions
    int code = ExecuteSpecialInstruction(instruction, computerState, outputFilePath);
    if(code)
        return (code == 2);

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
    return 0;
}
