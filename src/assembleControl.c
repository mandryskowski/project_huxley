#include "assembleControl.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>

#define DELIMETERS " ,"

void freeStrArray(char **strArray)
{
    int size = 0;
    char **ptr = strArray;
    while (*ptr != NULL)
    {
        size++;
        ptr++;
    }
    printf("%d\n", size);
    for (int i = 0; i < size; i++)
    {
        printf("%s xddd\n", strArray[i]);
        char* currentIntPtr = strArray[i];
        free(currentIntPtr);
    }
    free(strArray);
    exit(0);
}

// Checks if a string is contained in the array.
bool contains(char **list, char *element)
{
    char **ptr = list;
    while (*ptr != NULL)
    {
        if (!strcmp(*ptr, element))
        {
            return true;
        }
        ptr++;
    }
    return false;
}

char **getAlias(char **instruction)
{
    if (!instruction[0] || !instruction[1])
    {
        return instruction;
    }
    char **result;
    char *rzr = instruction[1][0] == 'w' ? "wzr" : "xzr";
    if (!strcmp(*instruction, "mul") || !strcmp(*instruction, "mneg"))
    {
        result = calloc(6 * sizeof(char *), 1);
        result[0] = !strcmp(*instruction, "mul") ? "madd" : "msub";
        result[1] = instruction[1];
        result[2] = instruction[2];
        result[3] = instruction[3];
        result[4] = rzr;
    }
    else
    {
        result = calloc(5 * sizeof(char *), 1);
        if (!strcmp(*instruction, "cmp") || !strcmp(*instruction, "cmn") || !strcmp(*instruction, "tst"))
        {
            printf("ass\n");
            result[0] = !strcmp(*instruction, "cmp") ? "subs" : (!strcmp(*instruction, "cmn") ? "adds" : "ands");
            result[1] = rzr;
            result[2] = instruction[1];
            result[3] = instruction[2];
        }
        else if (!strcmp(*instruction, "neg") || !strcmp(*instruction, "negs") || !strcmp(*instruction, "mvn")
        || !strcmp(*instruction, "mov"))
        {
            result[0] = !strcmp(*instruction, "neg") ? "sub" : (!strcmp(*instruction, "negs") ? "subs" :
                    (!strcmp(*instruction, "mvn") ? "orn" : "orr"));
            result[1] = instruction[1];
            result[2] = rzr;
            result[3] = instruction[2];
        }
        else
        {
            free(result);
            return instruction;
        }
    }
    printf("ass\n");
    freeStrArray(instruction);
    printf("assxd\n");
    free(rzr);
    return result;
}

// Interval is [start, end)
char *substr(char *string, int start, int end)
{
    assert(end < strlen(string) && start >= 0 && start < end);
    char *result = malloc((end - start + 1) * sizeof(char));
    strncpy(result, string + start, end - start);
    result[end - start] = '\0';
    printf("%s\n", result);
    return result;
}

// splits instruction into words
char **split(char *instruction)
{
    char **result = malloc(sizeof(char *) * 7);
    char **ptr = result;
    for (char *string = strtok(instruction, DELIMETERS); string != NULL; string = strtok(NULL, DELIMETERS))
    {
        //printf("%s lul\n", string);
        *ptr++ = string;
    }
    result[ptr - result] = NULL;
    return result;
}

void setBits(int *instruction, int mask, int start)
{
    *instruction |= mask << start;
}


assembleType getAssembleType(char *operation)
{

    char *DPops[] = {"add", "adds", "sub", "subs", "and", "ands", "bic", "bics",
                    "eor", "eon", "orr", "orn", "movn", "movk", "movz", "madd", "msub", NULL};
    char *BRANCHops[] = {"b", "br", NULL};
    char *SDTops[] = {"ldr", "str", NULL};
    char *SPECIALops[] = {"nop", "and", ".int", NULL};

    if (contains(DPops, operation)){
        return DP_ASS;
    }
    if (contains(BRANCHops, operation) ||
    (strlen(operation) > 1 && operation[0] == 'b' && operation[1] == '.'))
    {
        return BRANCH_ASS;
    }
    if (contains(SDTops, operation))
    {
        return SDT_ASS;
    }
    if (contains(SPECIALops, operation))
    {
        return SPECIAL_ASS;
    }
    return UNDEFINED_ASS;
}

int32_t assembleInstruction(char *instruction)
{
    printf("%s\n", instruction);
    char **tokenized = split(instruction);
    printf("%s\n", instruction);
    tokenized = getAlias(tokenized);
    printf("%s\n", instruction);
    char **ptr = tokenized;
    while (*ptr != NULL){
        printf("%s\n", *ptr);
        ptr++;
    }
    printf("%d\n", getAssembleType(*tokenized));
    exit(0);
    int32_t result;

    switch (getAssembleType(*tokenized))
    {
        case DP_ASS:
            //result = ...
            break;
        case BRANCH_ASS:
            //result = ...
            break;
        case SDT_ASS:
            //result = ...
            break;
        case SPECIAL_ASS:
            //result = ...
            break;
        default:
            perror("Unhandled assemble type");
            exit(EXIT_FAILURE);
    }

    freeStrArray(tokenized);
    return result;
}