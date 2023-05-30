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
	return calloc(1, sizeof(ComputerState));
}
/*
int main(int argc, char **argv) 
{
	FILE* fptr = fopen(argv[1], "r");
	if(fptr == NULL)
	{
		printf("File could not be opened");
		exit(1);
	}

	int32_t word;
	ComputerState *computerState = GenerateNewCS();

	while(read_word(fptr, &word))
	{
		ExecuteInstruction(word, computerState);
	}

	//Exception catching: if instruction set does not terminate with halt command,
	// 					  print final state of the machine
	generateOutputFile(computerState, OUTPUT_FILE_PATH);

	fclose(fptr);
	return 0;
} */
