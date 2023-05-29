#include "outputFileGenerator.h"

void generateOutputFile(ComputerState *computerState, char *outputFilePath)
{
    FILE *fptr = fopen(outputFilePath, "w");

    fprintf(fptr, "Registers:\n");
    for(int regIndex = 0; regIndex < 31; regIndex++) 
    {
        fprintf(fptr, "X%02d = %016x\n", regIndex, computerState -> registers[regIndex]);
    }

    fprintf(fptr, "PC = %016x\n", computerState->PC);

    fprintf(fptr, "PSTATE : %c%c%c%c\n", 
        ((computerState->pstate.nf) ? 'N' : '-'),
        ((computerState->pstate.zf) ? 'Z' : '-'),
        ((computerState->pstate.cf) ? 'C' : '-'),
        ((computerState->pstate.vf) ? 'V' : '-')
    );
    
    //Add print memory
    fprintf(fptr, "Non-zero memory:\n");
    for(int memAddr = 0; memAddr < MEMORY_SIZE; memAddr++) 
    {
        if(computerState->memory[memAddr])
            fprintf(fptr, "%08x: %08x\n", computerState->memory[memAddr]);

    }
    fclose(fptr);
}