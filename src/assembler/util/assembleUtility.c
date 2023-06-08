#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "assembleUtility.h"


#define DELIMETERS " ,"
#define MAX_INSTRUCTION_SIZE 6
#define SPECIAL_REGISTER 31

// Pre: array ends with a null.
void free2DArray(void **ptr)
{
    void **ptrCpy = ptr;
    while (*ptrCpy)
    {
        free(*ptrCpy);
        ptrCpy++;
    }
    free(ptr);
}

int find(char **list, char *element)
{
    int index = 0;
    while (list[index] != NULL)
    {
        if (!strcmp(list[index], element))
        {
            return index;
        }
        index++;
    }
    return -1; //Was not found
}

// Returns meaning of a given alias
void setAliasMeaning(char **instruction, int rzrPos)
{
    memmove(instruction + rzrPos + 1, instruction + rzrPos,
            (MAX_INSTRUCTION_SIZE - rzrPos - 1) * sizeof (char *));
    instruction[rzrPos] = instruction[1][0] == 'w' ? "wzr" : "xzr";
}

/* If a instruction is an alias it is replaced by its meaning.
 * Otherwise, it returns the given instruction itself.*/
void getAlias(char **instruction)
{
    char *alias[] = {"mul", "mneg", "cmp", "cmn", "tst", "neg", "negs",
                       "mvn","mov", NULL};
    char *meaning[] = {"madd", "msub", "subs", "adds", "ands", "sub", "subs",
                        "orn", "orr"};
    int index = find(alias, *instruction);

    // Given instruction is an alias. So we replace it with its meaning.
    if (index != -1)
    {
        instruction[0] = meaning[index];
        setAliasMeaning(instruction, (index < 2) ? 4 : (index < 5) ? 1 : 2);
    }
}

// Given shift shortcode, returns it's index
int getShiftCode(char *shiftID)
{
	char* shifts[] = {"lsl", "lsr", "asr", "ror"};
	for(int shiftCode = 0; shiftCode < sizeof(shifts) / sizeof(char *); shiftCode++)
    {
        if(!strcmp(shifts[shiftCode], shiftID))
        {
            return shiftCode;
        }
    }

	assert(0); //Invalid shiftCode
	return -1;
}

// Returns substring of string on positions [start, end)
char *substr(char *string, int start, int end)
{
    // Assert substring interval inside string limits
    assert(end <= strlen(string) && start >= 0 && start < end);

    // Allocate mem, copy to result, set end char on '\0'
    char *result = malloc((end - start + 1) * sizeof(char));
    strncpy(result, string + start, end - start);
    result[end - start] = '\0';

    return result;
}

// Returns string without first character
char *tail(char *string)
{
	return substr(string, 1, strlen(string));
}

// Return index of register from associated string
int getRegister(char *c)
{
	char *cTail = tail(c);
    int result = (strcmp(cTail, "zr")) ? strtol(cTail, NULL, 10) : SPECIAL_REGISTER;
    free(cTail);
	return result;
}

// Returns immediate value from associated string
int getImmediate(char *c)
{
	if(c[0] == '#')
    {
        char *cTail = tail(c);
        int result = strtol(cTail, NULL, 0);
        free(cTail);
        return result;
    }
	return strtol(c, NULL, 0);
}

// Splits instruction into words on DELIMITERS
char **split(char *instruction)
{
    char **result = calloc(sizeof(char *) * MAX_INSTRUCTION_SIZE, 1);
    char **ptr = result;
    for (char *string = strtok(instruction, DELIMETERS); string; string = strtok(NULL, DELIMETERS))
    {
        *ptr++ = string;
    }

    return result;
}

// Truncates bits if operation is 32 bit.
uint32_t truncateBits(uint32_t inputs, int bitCount)
{
	assert(bitCount < 32 && bitCount > 0);
	return inputs & ((1LL << bitCount) - 1);
}

// Sets bitmask in instruction starting at bit start.
void setBits(int *instruction, int mask, int start)
{
    *instruction |= mask << start;
}
