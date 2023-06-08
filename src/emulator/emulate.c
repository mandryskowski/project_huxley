#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>

#include "emulateControl.h"
#include "util/state.h"
#include "util/outputFileGenerator.h"
#include "instructions/immediateInstruction.h"

#define BUFSZ 4
#define BYTESZ 8

bool read_word(FILE *fptr, int32_t *word)
{
    return fread(word, sizeof(int), 1, fptr) == 1;
}

ComputerState* GenerateNewCS()
{
    ComputerState* state = calloc(1, sizeof(ComputerState));
    state->memory = calloc(MEMORY_SIZE, sizeof(char));
    state->pstate.zf = true;
    return state;
}

int main(int argc, char **argv)
{
    FILE* fptr = fopen(argv[1], "rb");
    if(fptr == NULL)
    {
        printf("File could not be opened\n");
        exit(1);
    }

    ComputerState *computerState = GenerateNewCS();
    fseek(fptr, 0, SEEK_END);
    long file_size = ftell(fptr);
    fseek(fptr, 0, SEEK_SET);

    fread(computerState->memory, sizeof(char), file_size, fptr);
    fclose(fptr);

    while(computerState->PC < file_size)
    {
        bool isTerminated = ExecuteInstruction(*(int32_t*)(computerState->memory + computerState->PC), computerState, argv[2]);
	if(isTerminated)
		break;
        computerState->PC += 4;
    }

    //Exception catching: if instruction set does not terminate with halt command,
    // 					  print final state of the machine
    generateOutputFile(computerState, argv[2]);

    free(computerState->memory);
    free(computerState);

    return 0;
}
