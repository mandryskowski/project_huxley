#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>

#define BUFSZ 4
#define BYTESZ 8

bool read_word(FILE *fptr, int32_t *word)
{
	return fread(word, sizeof(int), 1, fptr) == 1;
//	return fread(buf, size, nmemb, fptr) == BUFSZ;
}

int32_t read_word_old(unsigned char *buf)
{
	return *(int32_t*)buf;
/*	int32_t word = 0;
	for(int i=0;i<BUFSZ;i++)
	{
		word = (word << BYTESZ) + buf[i];
	}
	return word;
*/
}

int main(int argc, char **argv) 
{
    runTests();
	FILE* fptr = fopen(argv[1], "r");
	if(fptr == NULL)
	{
		printf("File could not be opened");
		exit(1);
	}
	int32_t word;
	while(read_word(fptr, &word))
	{
		//to:do Add opcode checking
		printf("0x%x\n", word);
	}
	fclose(fptr);
	return 0;
}
