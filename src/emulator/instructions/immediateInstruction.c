#include "immediateInstruction.h"
#include "../util/emulateUtility.h"
#include <stdio.h>
#include <stdlib.h>

void runAddition(bool sf, int opc, int rd, int64_t op, int64_t registerValue, ComputerState *computerState)
{
	// If subtraction make op negative
	if (getBits(1, 1, opc))
	{
	    op = -op;
	}
    uint64_t registerValueUnsigned = registerValue;
    if (!sf)
    {
        registerValue = (int32_t)registerValue;
        registerValueUnsigned = (uint32_t)registerValueUnsigned;
    }
    int64_t result =  registerValue + op;
    uint64_t resultUnsigned = registerValueUnsigned + op;

    if (!sf)
    {
        result = (int32_t)result;
        resultUnsigned = (uint32_t)resultUnsigned;
    }

    //Assigning result
    if (rd == 0b11111)
    {
        if (!getBits(0, 0, opc)){
            computerState->stack_ptr = (sf) ? result : (uint32_t)result;
        }
    }
    else
    {
        computerState->registers[rd] = (sf) ? result : (uint32_t)result;
    }

    // Updating flags for opc 01 and 11.
    if (getBits(0, 0, opc))
    {
        computerState->pstate.nf = result < 0;
        computerState->pstate.zf = result == 0;
        if (!sf)
        {
            computerState->pstate.vf = (registerValue ^ result) & ((int32_t)op ^ result) & INT32_MIN;
            computerState->pstate.cf = (registerValueUnsigned > resultUnsigned) || ((uint32_t)op > resultUnsigned);
        }
        else
        {
            computerState->pstate.vf = (registerValue ^ result) & (op ^ result) & INT64_MIN;
            computerState->pstate.cf = (registerValueUnsigned > resultUnsigned) || (op > resultUnsigned);
        }
	if(op == 0 && result >= 0 && getBit(1, opc))
	{
		computerState->pstate.cf = 1;
	}
    }
}

void ExecuteImmediate(int instruction, ComputerState * computerState)
{
    bool sf = getBits(31, 31, instruction);
    int opc = getBits(29, 30, instruction);
    int opi = getBits(23, 25, instruction);
    int rd = getBits(0, 4, instruction);
    //printf("%u\n", instruction);

    if (opi == 0b010)
    {
        //printf("%u\n", instruction);
        // opi is 010 then it is an arithmetic operation.
        bool sh = getBits(22, 22, instruction);
        int imm12 = getBits(10, 21, instruction);
        int rn = getBits(5, 9, instruction);
        int64_t op = sh ? imm12 << 12 : imm12;
        int64_t registerValue = (rn == 0b11111) ? computerState->stack_ptr : computerState->registers[rn];

        // If subtraction make op negative
        /*if (getBits(1, 1, opc))
        {
            op = -op;
        }*/

        runAddition(sf, opc, rd, op, registerValue, computerState);
    }
    else if (opi == 0b101)
    {
        //opi is 101 then it is a wide move
        int hw = getBits(21, 22, instruction);
        int shift = hw * 16;
        int64_t imm16 = getBits(5, 20, instruction);
        uint64_t op = imm16 << shift;
        uint64_t result;

        switch (opc)
        {
            case 0b00:
                result = ~op;
                break;
            case 0b10:
                result = op;
                break;
            case  0b11:
                result = computerState->registers[rd]
                        - (getBits(shift, shift + 15, computerState->registers[rd]) << shift) + op;
                break;
            default:
                fprintf(stderr, "Unsupported opcode: %d\n", opc);
                exit(EXIT_FAILURE);
        }

        if (!sf)
        {
            result = (uint32_t)result;
        }

        // Assume 11111 is not possible
        if (rd != 31)
            computerState->registers[rd] = (sf) ? result : (uint32_t)result;
    }
    else
    {
        fprintf(stderr, "Immediate instruction does not handle opi: %d\n", opi);
        exit(EXIT_FAILURE);
    }
}
