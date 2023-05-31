#include "registerInstruction.h"
#include "bitwiseInstruction.h"
#include "control.h"
#include <stdio.h>
#include <stdlib.h>
#include "immediateInstruction.h"


int64_t EncodedRegisterValue(ComputerState* computerState, int encodedReg)
{
    return (encodedReg == 0b11111) ? (ZR) : (computerState->registers[encodedReg]);
}

void MultiplyOperation(const int instruction, ComputerState* computerState,
                      int rn, int rm, int rd, bool sf)
{
    // This if should probably be moved to ExecuteRegister.
    // We should then consider how we'll set flags for this case.

    bool x = getBit(15, instruction);
    int ra = getBits(10, 14, instruction);
    const int bitCount = (sf) ? 63 : 31;

    const int64_t registerValueA = getBits(0, bitCount, EncodedRegisterValue(computerState, ra));
    const int64_t registerValueN = getBits(0, bitCount, EncodedRegisterValue(computerState, rn));
    const int64_t registerValueM = getBits(0, bitCount, EncodedRegisterValue(computerState, rm));
    const int64_t intermediateResult = registerValueN * registerValueM;

    int64_t result = (x) ? (registerValueA - intermediateResult) : (registerValueA + intermediateResult);

    result = getBits(0, bitCount, result);

    if(rd == 0b11111)
    {
        fprintf(stdout, "Load to Zero Register is ignored.\n");
        return;
    }

    computerState->registers[rd] = result;
}

void ExecuteRegister(int instruction, ComputerState* computerState) {
    
    bool sf = getBit(31, instruction);
    int opc = getBits(29, 30, instruction);
    int opr = getBits(21, 24, instruction);
    int rm = getBits(16, 20, instruction);
    int rn = getBits(5, 9, instruction);
    int rd = getBits(0, 4, instruction);
    bool M = getBit(28, instruction);
    
    if(M == 1)
    {
        MultiplyOperation(instruction, computerState, rn, rm, rd, sf);
	return;
    }

    else // M == 0
    {
        int shiftType = getBits(22, 23, instruction);
        int shiftAmount = getBits(10, 15, instruction);
        bool N = getBit(21, instruction);

        //For now hardcoded lsl, will be changed after 1.6 Bitwise Op is implemented
        int64_t op = EncodedRegisterValue(computerState, rm);
        ExecuteShift(shiftType, &op, shiftAmount, !sf);
 	    op = (N) ? (~op) : op;

        int64_t registerValue = (rn == 0b11111) ? ZR : computerState->registers[rn];
        int64_t result;

        if((opr & 0b1000) && !(opr & 0b0001)) 
        {
            //if opr matches 1xx0 then it is an arithmetic operation


            runAddition(sf, opc, rd, op, registerValue, computerState);
            return;
        }
        else
        {
            //if opr matches 0xxx then it is a logic operation

            switch(opc) {
                case 0b00: //and / bic
                    result = registerValue & op;
                    break;
                case 0b01: //orr / orn
                    result = registerValue | op;
                    break;
                case 0b10: //eon / eor
                    result = registerValue ^ op;
                    break;
                case 0b11: //ands / bics
                    result = registerValue & op;
                    break;
                default:
                    fprintf(stderr, "Unsupported opcode: %d\n", opc);
                    exit(EXIT_FAILURE);
            }
        }

        //Result assignation
        if(!sf)
        {
            result = getBits(0, 31, result);
        }
	
	//Update flags
	if(opc == 0b11)
	{
	        computerState->pstate.nf = (result < 0);
	        computerState->pstate.zf = (result == 0);
	        computerState->pstate.cf = 0;
	        computerState->pstate.vf = 0;
	}


        if(rd == 0b11111)
        {
            fprintf(stdout, "Load to Zero Register is ignored.\n");
            return;
        }
        else
        {
            computerState->registers[rd] = result;
        }
    }

}
