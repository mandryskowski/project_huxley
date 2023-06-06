#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <ctype.h>
#include "assembleControl.h"
#include "util/assembleUtility.h"

#define MAX_UINT64T UINT64_C(-1)

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
void processLabelTokens(char*** tokenized, char** tokens, Label* labels, char **toFreeEnd)
{
    while (*tokens != NULL)
    {
        uint64_t address = getLabelAddress(*tokens, labels);
        if (address != MAX_UINT64T)
        {
            *tokens = calloc(1, 19);
            *toFreeEnd++ = *tokens;
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

char *strsepP(char **stringp, const char *delim) {
    char *rv = *stringp;
    if (rv) {
        *stringp += strcspn(*stringp, delim);
        if (**stringp)
            *(*stringp)++ = '\0';
        else
            *stringp = 0; }
    return rv;
}

int main(int argc, char **argv) 
{
    FILE* input = fopen(argv[1], "rb+");
    int fileSize = getFileSize(input);
    char *data = malloc(fileSize + 1);
    fread(data, sizeof(char), fileSize, input);

    char ***tokenized = calloc(sizeof(char **) * (fileSize + 1), 1);
    char ***tokenizedPtr = tokenized;
    char **toFree = calloc(512, sizeof(char **));
    char **toFreeEnd = toFree;
    char *line;
    Label *label = calloc(128, sizeof(Label));
    Label *labelEnd = label;
    uint64_t address = 0;

    // First pass
    while ((line = strsepP(&data, "\n")))
    {
        // It's a comment or empty line therefore its irrelevant
        char **tokenizedLine = split(line);
        if (*line == '#' || *line == '\0' || *tokenizedLine == NULL)
        {
            continue;
        }

        int opLength = strlen(*tokenizedLine);
        // Is a label
        if (tokenizedLine[0][opLength - 1] == ':')
        {
            tokenizedLine[0][opLength - 1] = '\0';
            *labelEnd++ = (Label){*tokenizedLine, address};
            continue;
        }

        *tokenizedPtr++ = tokenizedLine;
        address += 4;
    }

    tokenizedPtr = tokenized;
    address = 0;
    FILE* output = fopen(argv[2], "w+");
    // Second pass
    while (*tokenizedPtr)
    {
        processLabelTokens(tokenizedPtr, *tokenizedPtr, label, toFreeEnd);
        uint32_t word = assembleInstruction(*tokenizedPtr, address);
        for (int i = 0; i < 32; i += 8)
        {
            fprintf(output, "%c", (unsigned char) (word >> i));
        }

        address += 4;
        tokenizedPtr++;
    }

    free2DArray((void **)tokenized);
    free2DArray((void **)toFree);
    free(data);
    free(label);
    fclose(input);
    fclose(output);
    return 0;
}
