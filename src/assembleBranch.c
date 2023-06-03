#include "assembleControl.h"
#include "assembleBranch.h"
#include <stdint.h>

int32_t branchOpcode(char** tokenized, BOperation op, int64_t PC) 
{
    int32_t instruction = 0;
    int32_t cond;
    int32_t simm;

    //UNCONDITIONAL
    if (op == B_UNCOND) 
    {
        simm = truncateBits((getImmediate(tokenized[1]) - PC) >> 2, 26);
        setBits(&instruction, simm, 0);
        setBits(&instruction, 0b000101, 26);
    }

    //REGISTER
    if (op == B_REG)
    {
        setBits(&instruction, 0b00000, 0);
        int32_t reg = getRegister(tokenized[1]);
        setBits(&instruction, reg, 5);
        setBits(&instruction, 0b1101011000011111000000, 10);
    }

    //CONDITIONAL
    if(op == B_COND)
    {
       
        char* condition = substr(tokenized[0], 2, 4);
        char* mnemonic[] = {"eq", "ne", "ge", "lt", "gt", "le", "al", NULL};
        int encoding[] = {0b0000, 0b0001, 0b1010, 0b1011, 0b1100, 0b1101, 0b1110};

        //FIND CONDITION
        cond = encoding[find(mnemonic, condition) - 1];

        setBits(&instruction, cond, 0);
        setBits(&instruction, 0b0, 4);
        simm = truncateBits((getImmediate(tokenized[1]) - PC) >> 2, 19);
        setBits(&instruction, simm, 5);
	    setBits(&instruction, 0b01010100, 24);
    }

    return instruction;
}

