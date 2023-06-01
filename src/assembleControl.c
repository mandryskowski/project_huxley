#include "assembleControl.h"
#include "assembleSpecial.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>

#define DELIMETERS " ,"

// Checks if a string is contained in the array.
bool contains(char **list, char *element)
{
    char **ptr = list;
    while (*ptr)
    {
        if (!strcmp(*ptr++, element))
        {
            return true;
        }
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
    free(instruction);
    return result;
}

// Interval is [start, end)
char *substr(char *string, int start, int end)
{
    assert(end < strlen(string) && start >= 0 && start < end);
    char *result = malloc((end - start + 1) * sizeof(char));
    strncpy(result, string + start, end - start);
    result[end - start] = '\0';
    return result;
}

// splits instruction into words
char **split(char *instruction)
{
    char **result = malloc(sizeof(char *) * 6);
    char **ptr = result;
    for (char *string = strtok(instruction, DELIMETERS); string != NULL; string = strtok(NULL, DELIMETERS))
    {
        *ptr++ = string;
    }
    return result;
}

void setBits(int *instruction, int mask, int start)
{
    *instruction |= mask << start;
}


assembleType getAssembleType(char **operation)
{

    char *DPops[] = {"add", "adds", "sub", "subs", "and", "ands", "bic", "bics",
                    "eor", "eon", "orr", "orn", "movn", "movk", "movz",
                    "madd", "msub", NULL};
    char *BRANCHops[] = {"b", "br", NULL};
    char *SDTops[] = {"ldr", "str", NULL};
    char *SPECIALops[] = {"nop", ".int", NULL};

    if ((operation[3] && !strcmp(*operation, "and") && !strcmp(operation[3], "x0")) ||
    contains(SPECIALops, *operation))
    {
        return SPECIAL_ASS;
    }
    if (contains(DPops, *operation)){
        return DP_ASS;
    }
    if (contains(BRANCHops, *operation) ||
    (strlen(*operation) > 1 && (*operation)[0] == 'b' && (*operation)[1] == '.'))
    {
        return BRANCH_ASS;
    }
    if (contains(SDTops, *operation))
    {
        return SDT_ASS;
    }
    return UNDEFINED_ASS;
}

int32_t assembleInstruction(char *instruction)
{
    char **tokenized = split(instruction);
    tokenized = getAlias(tokenized);
    int type = getAssembleType(tokenized);
    printf("%d\n", type);
    exit(0);
    int32_t result;

    exit(0);
    switch (type)
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

    free(tokenized);
    return result;
}