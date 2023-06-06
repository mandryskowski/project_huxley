#include "../util/assembleUtility.h"
#include "assembleBranch.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h> 

int32_t findCondition(char* branchCond)
{
    // Types of conditions
    char* mnemonic[] = {"eq", "ne", "ge", "lt", "gt", "le", "al"};

    // Encodings respectively: 0b0000, 0b0001, 0b1010, 0b1011, 0b1100, 0b1101, 0b1110
    int encoding[] = {0x0, 0x1, 0xA, 0xB, 0xC, 0xD, 0xE};

    for(int i = 0; i < sizeof(mnemonic) / sizeof(char *); i++)
    {
        if(!strcmp(branchCond + 2, mnemonic[i]))
        {
            return encoding[i];
        }
    }

    printf("Condition %s does not exist!\n", branchCond);
    exit(EXIT_FAILURE);

}

int32_t assembleBranch(char** tokenized, BOperation op, int64_t PC) 
{
    int32_t instruction = 0;

    //UNCONDITIONAL
    if (op == B_UNCOND) 
    {
        int32_t simm = truncateBits((getImmediate(tokenized[1]) - PC) >> 2, 26);
        setBits(&instruction, simm, 0); //Sets simm on bits 0-25
        setBits(&instruction, 0x05, 26); //Sets b opcode on bits 26-31 (mask 0b000101)
    }

    //REGISTER 
    if (op == B_REG)
    {
        int32_t reg = getRegister(tokenized[1]);
        setBits(&instruction, reg, 5); //Sets reg on bits 5-9
        setBits(&instruction, 0x3587C0, 10); //Sets breg opcode on bits 10-31 (mask 0b1101011000011111000000)
    }

    //CONDITIONAL
    if(op == B_COND)
    {
        
        setBits(&instruction, findCondition(tokenized[0]), 0); //Sets cond on bits 0-3
        int32_t simm = truncateBits((getImmediate(tokenized[1]) - PC) >> 2, 19);
        setBits(&instruction, simm, 5); //Sets simm on bits 5-23
	    setBits(&instruction, 0x54, 24); //Sets bcond opcode on bits 24-31 (mask 0b01010100)
    }

    return instruction;
}

