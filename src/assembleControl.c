#include "assembleControl.h"
#include "assembleSpecial.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include "assembleDPI.h"
#include "assembleSDT.h"
#include "branchAssemble.h"
#include "label.h"
#define DELIMETERS " ,"

typedef struct TypePair {
	assembleType aType;
	int opcode;
} TypePair;

TypePair *newTypePair(assembleType aType, int opcode)
{
	TypePair *p = malloc(sizeof(TypePair));
	p->aType = aType;
	p->opcode = opcode;
	return p;
}

void freeStrArray(char **strArray)
{
    int size = 0;
    char **ptr = strArray;
    while (*ptr != NULL)
    {
        size++;
        ptr++;
    }
    //printf("%d\n", size);
    for (int i = 0; i < size; i++)
    {
        //printf("%s xddd\n", strArray[i]);
        char* currentIntPtr = strArray[i];
        free(currentIntPtr);
    }
    free(strArray);
    exit(0);
}

// Checks if a string is contained in the array.
int find(char **list, char *element)
{
    int index = 0;
    while (list[index] != NULL)
    {
        if (!strcmp(list[index], element))
        {
            return index + 1;
        }
        index++;
    }
    return 0; //Was not found
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
        result = calloc(6 * sizeof(char *), 1);
        if (!strcmp(*instruction, "cmp") || !strcmp(*instruction, "cmn") || !strcmp(*instruction, "tst"))
        {
            result[0] = !strcmp(*instruction, "cmp") ? "subs" : (!strcmp(*instruction, "cmn") ? "adds" : "ands");
            result[1] = rzr;
            result[2] = instruction[1];
            result[3] = instruction[2];
            result[4] = instruction[3];
            result[5] = instruction[4];
        }
        else if (!strcmp(*instruction, "neg") || !strcmp(*instruction, "negs") || !strcmp(*instruction, "mvn")
        || !strcmp(*instruction, "mov"))
        {
            result[0] = !strcmp(*instruction, "neg") ? "sub" : (!strcmp(*instruction, "negs") ? "subs" :
                    (!strcmp(*instruction, "mvn") ? "orn" : "orr"));
            result[1] = instruction[1];
            result[2] = rzr;
            result[3] = instruction[2];
            result[4] = instruction[3];
            result[5] = instruction[4];
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
    assert(end <= strlen(string) && start >= 0 && start < end);
    char *result = malloc((end - start + 1) * sizeof(char));
    strncpy(result, string + start, end - start);
    result[end - start] = '\0';

    return result;
}

char *tail(char *string)
{
	return substr(string, 1, strlen(string));
}

int getRegister(char *c)
{
	c = tail(c);
	return (strcmp(c, "zr")) ? atoi(c) : 0b11111;
}

int stoi(char *string)
{
        if(strlen(string) < 2)
                return strtol(string, NULL, 10);
        if(!strcmp("0x", substr(string, 0, 2)))
                return strtol(string, NULL, 16);
        return strtol(string, NULL, 10);
}

int getImmediate(char *c)
{
	if(c[0] == '#')
		c = tail(c);
	return stoi(c);
}

// splits instruction into words
char **split(char *instruction)
{
    char **result = calloc(sizeof(char *) * 6, 1);
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


TypePair *getAssembleType(char **operation)
{

    char *DPops[] = {"add", "adds", "sub", "subs", "and", "bic",
                    "orr", "orn", "eor", "eon", "ands", "bics",
                    "movn","movmov", "movz", "movk", "madd", "msub", NULL};
    char *BRANCHops[] = {"b", "br", NULL};
    char *SDTops[] = {"ldr", "str", NULL};
    char *SPECIALops[] = {"nop", ".int", NULL};

    int result;

    if ((result = find(SPECIALops, *operation)))
    {
        return newTypePair(SPECIAL_ASS, result - 1);
    }
    else if (operation[3] && !strcmp(*operation, "and") && !strcmp(operation[3], "x0"))
    {
        return newTypePair(SPECIAL_ASS, 2);
    }
    if ((result = find(DPops, *operation))){
        return newTypePair(DP_ASS, result - 1);
    }
    if ((result = find(BRANCHops, *operation)))
    {
        return newTypePair(BRANCH_ASS, result - 1);
    } 
    else if((strlen(*operation) > 1 && (*operation)[0] == 'b' && (*operation)[1] == '.'))
    {
            return newTypePair(BRANCH_ASS, 2);
    }   
    if ((result = find(SDTops, *operation)))
    {
        return newTypePair(SDT_ASS, result - 1);
    }
    return newTypePair(UNDEFINED_ASS, 0);
}

int32_t assembleInstruction(char **tokenized, uint64_t PC)
{
    tokenized = getAlias(tokenized);
    TypePair *tp = getAssembleType(tokenized);
    int32_t result = 0;

    switch (tp->aType)
    {
        case DP_ASS:
            result = assembleDPI(tokenized, (DPOperation)(tp->opcode));
            break;
        case BRANCH_ASS:
            result = branchOpcode(tokenized, (BOperation)(tp->opcode), PC);
            break;
        case SDT_ASS:
            result = assembleSDT(tokenized, (SDTOperation)(tp->opcode), PC);
            break;
        case SPECIAL_ASS:
            result = assembleSpecial(tokenized, tp->opcode);
            break;
        default:
	    printf("-%s-", tokenized[0]);
            perror("Unhandled assemble type");
            exit(EXIT_FAILURE);
    }

    free(tokenized);
    return result;
}
