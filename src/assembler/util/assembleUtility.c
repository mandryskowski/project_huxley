#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "assembleUtility.h"


#define DELIMETERS " ,"
#define MAX_INSTRUCTION_SIZE 6
#define SPECIAL_REGISTER 31

/* Finds index of an element in the list equal to (char *element).
 * if the element is not found returns 0.*/
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

/* returns meaning of a given alias*/
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
    if (index)
    {
        instruction[0] = meaning[index - 1];
        setAliasMeaning(instruction, (index < 3) ? 4 : (index < 6) ? 1 : 2);
    }
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

// returns the 2nd largest suffix of a string.
char *tail(char *string)
{
	return substr(string, 1, strlen(string));
}

// Return index of register from associated string.
int getRegister(char *c)
{
	c = tail(c);
	return (strcmp(c, "zr")) ? strtol(c, NULL, 10) : SPECIAL_REGISTER;
}

// Returns immediate value from associated string.
int getImmediate(char *c)
{
	if(c[0] == '#')
		c = tail(c);
	return strtol(c, NULL, 0);
}

// splits instruction into words
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

/*Truncates bits if operation is 32 bit.*/
uint32_t truncateBits(uint32_t inputs, int bitCount)
{
	assert(bitCount < 32 && bitCount > 0);
	return inputs & ((1LL << bitCount) - 1);
}

/* Sets bitmask starting at bit start.*/
void setBits(int *instruction, int mask, int start)
{
    *instruction |= mask << start;
}