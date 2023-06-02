#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <string.h>
#include <ctype.h>
#include "assembleDPI.h"
#include "label.h"
#include "assembleControl.h"

void makeStrLowercase(char* str)
{
	for (; *str != '\0'; str++)
	{
		*str = tolower(*str);
	}
}

int main(int argc, char **argv) 
{
	FILE* fptr = fopen(argv[1], "rb+");
	FILE* outfptr = fopen(argv[2], "w+");
	if(fptr == NULL)
	{
		printf("Input file could not be opened\n");
		exit(1);
	}
	if (outfptr == NULL)
	{
		printf("Output file could not be opened\n");
		exit(1);
	}

	fseek(fptr, 0, SEEK_END);
        long input_file_size = ftell(fptr);
        fseek(fptr, 0, SEEK_SET);

	char* fileStr = malloc(input_file_size + 1);
	char endln = '\n';
	if(*(fileStr + input_file_size - 1) != endln)
	{
		strncat(fileStr, &endln, 1);
		input_file_size++;
	}
	fread(fileStr, sizeof(char), input_file_size, fptr);
//	fclose(fptr); // close input file

	Label labels[128];
	// 1st pass: getting labels' addresses
	{
		Label* curLabel = labels;
		char* curLine = fileStr;
		int64_t curAddress = 0;
		while(curLine != NULL)
		{
			char* nextLine = strchr(curLine, '\n');
			size_t len = (nextLine != NULL) ? (nextLine - curLine) : (strlen(curLine));

			if (len > 0 && curLine[len - 1] == ':' && *curLine != '#')
			{
				curLabel->address = curAddress;
				curLabel->name = malloc(len);
				memset(curLabel->name, '\0', len);
				strncpy(curLabel->name, curLine, len - 1); // copy from curLine to curLabel's name but ignore the colon.
				memset(curLine, '#', len); // we can comment out this line as we've read the label and it is not an instruction
			}

			curAddress += 4;
			curLabel++;
			curLine = (nextLine != NULL) ? (nextLine + 1) : NULL;
		}

		while (curLabel != labels + 128)
		{
			curLabel->name = NULL;
			curLabel->address = 0;
			curLabel++;
		}
	}

	fseek(fptr, 0, SEEK_SET);
	
	FILE* outputFileBegin = outfptr;
	// 2nd pass: translation to binary file
	{
		char* curLine = fileStr;
		uint64_t currPC = 0;
		while (curLine != NULL)
		{
			char* nextLine = strchr(curLine, '\n');
			size_t len = (nextLine != NULL) ? (nextLine - curLine) : (strlen(curLine));
			if (len > 0 && *curLine != '#') // ignore empty or commented lines
			{
				char* str = malloc(len);
				memset(str, '\0', len + 1);
				strncpy(str, curLine, len);
				makeStrLowercase(str);

				uint32_t word = assembleInstruction(str, labels, currPC);
				for (int i = 0; i < 32; i += 8)
				{
					fprintf(outfptr, "%c", (unsigned char) (word >> i));
				}

				printf("%s: %08x\n", str, word);
				puts(str);
				currPC += 4;
			}
			curLine = (nextLine != NULL) ? (nextLine + 1) : NULL;
		}
	}
	
	fclose(outputFileBegin);
	return 0;
}

