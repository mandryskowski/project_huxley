#include "registerInstruction.h"
#include "control.h"
#include <stdio.h>
#include <stdlib.h>


int64_t EncodedRegisterValue(ComputerState* computerState, int encodedReg)
{
    return (encodedReg == 0b11111) ? (computerState->zr) : (computerState->registers[encodedReg]);
}

bool MultiplyOperation(const int instruction, ComputerState* computerState,
                      int ra, int rn, int rm, int rd)
{
    // This if should probably be moved to ExecuteRegister.
    // We should then consider how we'll set flags for this case.
    if(rd == 0b11111)
    {
        fprintf("Load to Zero Register is ignored.\n");
        return;
    }

    bool x = getBits(15, 15, instruction);
    int ra = getBits(10, 14, instruction);
    const int bitCount = sf ? 64 : 32;
    
    const int64_t registerValueA = getBits(0, bitCount, EncodedRegisterValue(computerState, ra));
    const int64_t registerValueN = EncodedRegisterValue(computerState, rn);
    const int64_t registerValueM = EncodedRegisterValue(computerState, rm);
    const int64_t intermediateResult = registerValueN * registerValueM;

    int64_t result = (x) ? (registerValueA - intermediateResult) : (registerValueA + intermediateResult);

    computerState->registers[rd] = getBits(0, bitCount, result);
}

void ExecuteRegister(int instruction, ComputerState* computerState) {
    
    bool sf = getBits(31, 31, instruction);
    int opc = getBits(29, 30, instruction);
    int opr = getBits(21, 24, instruction);
    int rm = getBits(16, 20, instruction);
    int rn = getBits(5, 9, instruction);
    int rd = getBits(0, 4, instruction);
    
    if(opr == 0b1000 && opc == 0b00)
    {
        MultiplyOperation(instruction, computerState, ra, rn, rm, rd);
    }

    else 
    {
        int shift = getBits(22, 23, instruction);
        int imm6 = getBits(10, 15, instruction);
        bool N = getBits(21, 21, instruction);

        //For now hardcoded lsl, will be changed after 1.6 Bitwise Op is implemented
        int64_t op = N ? ~(imm6 << shift) : (imm6 << shift);
        int64_t registerValue = (rn == 0b11111) ? computerState -> zr : computerState->registers[rn];
        int64_t registerValueUnsigned = (rn == 0b11111) ? computerState -> zr : computerState->registers[rn];
        int64_t result;
        int64_t resultUnsigned;

        if((opr & 0b1000) && !(opr & 0b0001)) 
        {
            //if opr matches 1xx0 then it is an arithmetic operation
        
            result = getBits(1, 1, opc) ? registerValue - op : registerValue + op;
            resultUnsigned = getBits(1, 1, opc) ? registerValueUnsigned - op : registerValueUnsigned + op;
            if(getBits(0, 0, opc))
            {
                computerState->pstate.nf = (result < 0);
                computerState->pstate.zf = (result == 0);

                if(!sf)
                {
                    computerState->pstate.cf = ((registerValueUnsigned ^ resultUnsigned) & (op ^ resultUnsigned)) >> 31;
                    computerState->pstate.vf = (registerValue ^ result) & (op ^ result) & INT32_MIN;
                }
                else
                {
                    computerState->pstate.cf = ((registerValueUnsigned ^ resultUnsigned) & (op ^ resultUnsigned)) >> 63;
                    computerState->pstate.vf = (registerValue ^ result) & (op ^ result) & INT64_MIN;
                }
            }
        }
        else if(!(opr & 0b1000)) 
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
                    //Update flags
                    {
                        computerState->pstate.nf = (result < 0);
                        computerState->pstate.zf = (result == 0);
                        computerState->pstate.cf = 0;
                        computerState->pstate.vf = 0;
                    }
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

        if(rd == 0b11111)
        {
            computerState->zr = result;
        }
        else
        {
            computerState->registers[rd] = result;
        }
    }


}