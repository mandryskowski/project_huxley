#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <ctype.h>
#include "label.h"
#include "assembleControl.h"

void makeStrLowercase(char* str)
{
	for (; *str != '\0'; str++)
	{
		*str = tolower(*str);
	}
}

// Replace all tokens matching a label with their corresponding addresses. Convert the rest to lowercase.
void processLabelTokens(char** tokens, Label* labels)
{
    while (*tokens != NULL)
    {
        uint64_t address = getLabelAddress(*tokens, labels);
        //printf("address of token %s: %d \n", *tokens, address);
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

int getFileSize(FILE *file)
{
    fseek(file, 0, SEEK_END);
    int result = ftell(file);
    rewind(file);

    return result;
}

int main(int argc, char **argv) 
{
    FILE* input = fopen(argv[1], "rb+");
    int fileSize = getFileSize(input);
    char *data = malloc(fileSize + 1);
    fread(data, sizeof(char), fileSize, input);
    char ***tokenized = calloc(sizeof(char **) * (fileSize + 1), 1);
    char ***tokenizedPtr = tokenized;
    uint64_t address = 0;
    Label *label = calloc(128, sizeof(Label));
    Label *labelEnd = label;
    char *line;

    // First pass
    while ((line = strsep(&data, "\n")))
    {
        // It's a comment or empty line therefore its irrelevant
        if (*line == '#' || *line == '\0')
        {
            continue;
        }
        *tokenizedPtr = split(line);
        char *operation = **tokenizedPtr;
        int opLength = strlen(operation);
        // Is a label
        if (operation[opLength - 1] == ':')
        {
            *labelEnd++ = (Label){substr(operation, 0, opLength - 1), address};
            continue;
        }

        tokenizedPtr++;
        address += 4;
    }

    tokenizedPtr = tokenized;
    address = 0;
    FILE* output = fopen(argv[2], "w+");
    // Second pass
    while (*tokenizedPtr)
    {
        processLabelTokens(*tokenizedPtr, label);
        uint32_t word = assembleInstruction(*tokenizedPtr, address);
        for (int i = 0; i < 32; i += 8)
        {
            fprintf(output, "%c", (unsigned char) (word >> i));
        }

        //printf("%08x\n", word);
        address += 4;
        tokenizedPtr++;
    }

    free(data);
    free(tokenized);
    free(label);
    fclose(input);
    fclose(output);
    return 0;
}
