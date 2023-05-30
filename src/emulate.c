#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>

#include "control.h"
#include "state.h"
#include "outputFileGenerator.h"

#define BUFSZ 4
#define BYTESZ 8

bool read_word(FILE *fptr, int32_t *word)
{
	return fread(word, sizeof(int), 1, fptr) == 1;
}

ComputerState* GenerateNewCS() 
{
	ComputerState* state = calloc(1, sizeof(ComputerState));
	state->memory = malloc(MEMORY_SIZE);
	memset(state->memory, 0, MEMORY_SIZE);
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

	int32_t word;
	ComputerState *computerState = GenerateNewCS();
//	fread(&word, 1, 1, fptr);
	while(read_word(fptr, &word))
	{
		printf("now executing: 0x%x\n", word);
		ExecuteInstruction(word, computerState, argv[2]);
		printf("executed instr\n");
	}
	printf("emulation finished");
	//Exception catching: if instruction set does not terminate with halt command,
	// 					  print final state of the machine
	generateOutputFile(computerState, argv[2]);

	fclose(fptr);
	return 0;
}
