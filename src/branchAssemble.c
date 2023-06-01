#include "assembleControl.h"
#include "branchAssemble.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>



int32_t getSimm(char* c,Label* labels) 
{
    int32_t simm = 0;
    if(c[0] == '0')
    {
        char* ptr;
        simm = (int32_t) strtol(c, &ptr, 0);
    }
    
    else
    {
        simm = getLabelAddress(c, labels);
    }

    return simm;
}

int32_t branchOpcode(char** tokenized, Label* labels, BOperation op) 
{
    int32_t instruction = 0;
    int32_t cond = 0;
    int32_t simm = 0;
    //char** tokenized = split(c);

    //UNCONDITIONAL
    if (op == B_UNCOND) 
    {
        simm = getSimm(tokenized[1], labels);
        setBits(&instruction, simm, 0);
        setBits(&instruction, 0b000101, 26);
    }

    //REGISTER
    if (op == B_REG)
    {
        setBits(&instruction, 0b00000, 0);
        int32_t reg = tokenized[0][1] - '0';
        setBits(&instruction, reg, 5);
        setBits(&instruction, 0b1101011000011111000000, 10);
    }

    //CONDITIONAL
    if(op == B_COND)
    {
       
        char* condition = substr(   tokenized[0], 2, 4);
        char* mnemonic[] = {"eq", "ne", "ge", "lt", "gt", "le", "al"};
        int32_t encoding[] = {0b0000, 0b0001, 0b1010, 0b1011, 0b1100, 0b1101, 0b1110};
        
        //FIND CONDITION
        for(int i = 0; i < 7; i++)
        {
            if(!strcmp(condition, mnemonic[i]))
            {
                cond = encoding[i];
            }
        }

        setBits(&instruction, cond, 0);
        setBits(&instruction, 0b0, 4); 
        simm = getSimm(tokenized[0], labels);
        setBits(&instruction, simm, 5);                  
    }


    return instruction;
}

