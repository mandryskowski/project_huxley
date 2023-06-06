#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <string.h>
#include <ctype.h>
#include "assembleControl.h"
#include "instructions/assembleDPI.h"
#define MAX_UINT64T UINT64_C(-1)

void makeStrLowercase(char* str)
{
	for (; *str != '\0'; str++)
	{
		*str = tolower(*str);
	}
}

typedef struct Label
{
	char* name;
        int64_t address;
} Label;     

// Returns the address associated with the given labelName.
// You must pass the array containing all known labels.
// If no matching label is found, the function returns MAX_UINT64T.
uint64_t getLabelAddress(char* name, Label* label)
{
	while (label->name != NULL)
	{
		if (!strcmp(name, label->name))
		{
			return label->address;
		}
		label++;
	}

	return MAX_UINT64T; // if not found
}


// Replace all tokens matching a label with their corresponding addresses. Convert the rest to lowercase.
void processLabelTokens(char** tokens, Label* labels)
{
        while (*tokens != NULL)
        {
                uint64_t address = getLabelAddress(*tokens, labels);

                if (address == MAX_UINT64T) // if this is not a label, make it lowercase.
				{
					makeStrLowercase(*tokens);
				}
				else
				{
					sprintf(*tokens, "#%" PRIu64, address); // replace label token with #address.
				}
				
				tokens++;
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
			char* thisLineStr = malloc(len);
		
			if (len == 0 || *curLine == '#') // skip if empty/commented line
			{
				curLine = (nextLine != NULL) ? (nextLine + 1) : NULL;
				continue;
			}

			memset(thisLineStr, '\0', len + 1);
			strncpy(thisLineStr, curLine, len);
			printf("%s\n", thisLineStr);
			// Remove trailing whitespaces
			{
				size_t leadingWhitespaceLength = strspn(thisLineStr, " ");

				if (leadingWhitespaceLength == len)
				{
					len = 0;
				}

				char* whitespace = strchr(thisLineStr + leadingWhitespaceLength, ' ');
				if (whitespace != NULL)
				{
					len = whitespace - thisLineStr;
					thisLineStr[len] = '\0';
				}

			}

			if (len == 0) // comment out line consisting of just whitespace
			{
				*curLine = '#';
			}
			else if (thisLineStr[len - 1] == ':') // check if this line represents a label
			{
				thisLineStr[len - 1] = '\0'; // get rid of the colon at the end.
				curLabel->address = curAddress;
				curLabel->name = thisLineStr;
				*curLine = '#'; // we can comment out this line as we've read the label and it is not an instruction
				curLabel++;
			}
			else // otherwise it's an instruction
			{
				curAddress += 4;
			}

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
				char** tokenized = split(str);
				processLabelTokens(tokenized, labels);
				uint32_t word = assembleInstruction(tokenized, currPC);
				
				for (int i = 0; i < 32; i += 8)
				{
					fprintf(outfptr, "%c", (unsigned char) (word >> i));
				}

				currPC += 4;
			}
			curLine = (nextLine != NULL) ? (nextLine + 1) : NULL;
		}
	}
	
	fclose(outputFileBegin);
	return 0;
}

