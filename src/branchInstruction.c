#include <stdio.h>
#include <stdlib.h>
#include "branchInstruction.h"
#include "state.h"
#include "control.h"

bool condHolds(int instruction, ComputerState *computerState)
{
    int cond = getBits(0, 4, instruction);
    const struct Pstate pstate = computerState->pstate;

    switch (cond)
    {
        case 0b0000:
            return pstate.zf == 1;
        case 0b0001:
            return pstate.zf == 0;
        case 0b1010:
            return pstate.nf == 1;
        case 0b1011:
            return pstate.nf != 1;
        case 0b1100:
            return pstate.zf == 0 && pstate.nf == pstate.vf;
        case 0b1101:
            return !(pstate.zf == 0 && pstate.nf == pstate.vf);
        case 0b1110:
            return 1;
        default:
            fprintf(stderr, "Unhandled condition opcode for branch");
            exit(EXIT_FAILURE);
    }
}

void ExecuteBranch(int instruction, ComputerState *computerState){

    int opc = getBits(30, 31, instruction);
    int64_t offset;

    switch (opc)
    {
        case 0b00:
            offset = getBitsSignExt(0, 25, instruction) << 2;
            break;
        case 0b11:
            {
                int rd = getBits(5, 9, instruction);
                if (rd == 31){
                    fprintf(stderr, "register 31 does not exist");
                    exit(EXIT_FAILURE);
            }
            offset = computerState->registers[rd] - computerState->stack_ptr;}
            break;
        case 0b01:
            offset = condHolds(instruction, computerState) ? getBitsSignExt(5, 23, instruction) << 2 : 0;
        default:
            fprintf(stderr, "Unhandled opcode for branch instruction");
            exit(EXIT_FAILURE);
    }

    computerState->stack_ptr += offset;
}
