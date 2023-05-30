#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>

#include "control.h"
#include "state.h"
#include "outputFileGenerator.h"
#include "immediateInstruction.h"

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
	printf("fsize %d \n", file_size);
	fread(computerState->memory, sizeof(char), file_size, fptr);

	while(computerState->PC < file_size || 1)
	{
		ExecuteInstruction(*(int32_t*)(computerState->memory + computerState->PC), computerState, argv[2]);
		printf("executed instr\n");
		computerState->PC += 4;
	}
	printf("emulation finished");
	//Exception catching: if instruction set does not terminate with halt command,
	// 					  print final state of the machine
	generateOutputFile(computerState, argv[2]);

	fclose(fptr);
	free(computerState->memory);
	free(computerState);
	return 0;
}
