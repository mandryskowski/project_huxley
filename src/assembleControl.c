#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "assembleDPI.h"
#include "assembleSDT.h"
#include "assembleBranch.h"
#include "assembleControl.h"
#include "assembleSpecial.h"
#include "label.h"

#define DELIMETERS " ,"
#define MAX_INSTRUCTION_SIZE 6
#define SPECIAL_REGISTER 31

// Maintains general instruction type and code within aType
typedef struct TypePair {
	assembleType aType;
	int opcode;
} TypePair;

// Constructon for TypePair
TypePair *newTypePair(assembleType aType, int opcode)
{
	TypePair *p = malloc(sizeof(TypePair));
	p->aType = aType;
	p->opcode = opcode;
	return p;
}

/* Finds index of an element in the list equal to (char *element).
 * if the element is not found returns -1.*/
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
    //Assert substring interval inside string limits
    assert(end <= strlen(string) && start >= 0 && start < end);

    //Allocate mem, copy to result, set end char on '\0'
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
	c = tail(c);
	return (strcmp(c, "zr")) ? strtol(c, NULL, 10) : SPECIAL_REGISTER;
}

// Returns immediate value from associated string
int getImmediate(char *c)
{
	if(c[0] == '#')
		c = tail(c);
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

// Truncates bits if operation is 32 bit
uint32_t truncateBits(uint32_t inputs, int bitCount)
{
	assert(bitCount < 32 && bitCount > 0);
	return inputs & ((1LL << bitCount) - 1);
}

// Sets bits in instruction to mask starting at bit start
void setBits(int *instruction, int mask, int start)
{
    *instruction |= mask << start;
}

// Returns a type and index of given operation
TypePair *getAssembleType(char **operation)
{

    //Define all operation types
    char *DPops[] = {"add", "adds", "sub", "subs", "and", "bic",
                    "orr", "orn", "eor", "eon", "ands", "bics",
                    "movn","movmov", "movz", "movk", "madd", "msub", NULL};
    char *BRANCHops[] = {"b", "br", NULL};
    char *SDTops[] = {"ldr", "str", NULL};
    char *SPECIALops[] = {"nop", ".int", NULL};

    int result = 0;

    //Special op
    if ((result = find(SPECIALops, *operation)) != -1)
    {
        return newTypePair(SPECIAL_ASS, result);
    }
    else if (operation[3] && !strcmp(*operation, "and") && !strcmp(operation[3], "x0"))
    {
        return newTypePair(SPECIAL_ASS, 2);
    }

    //DP op
    if ((result = find(DPops, *operation)) != -1){
        return newTypePair(DP_ASS, result);
    }

    //Branch op
    if ((result = find(BRANCHops, *operation)) != -1)
    {
        return newTypePair(BRANCH_ASS, result);
    }
    else if((strlen(*operation) > 1 && (*operation)[0] == 'b' && (*operation)[1] == '.'))
    {
            return newTypePair(BRANCH_ASS, 2);
    }

    //SDT op
    if ((result = find(SDTops, *operation)) != -1)
    {
        return newTypePair(SDT_ASS, result);
    }

    return newTypePair(UNDEFINED_ASS, 0);
}

// Assembles given instruction.
int32_t assembleInstruction(char **tokenized, uint64_t PC)
{
    /* Checks if the instruction has at least 1 word. If it does not
     * then it is incorrect instruction. Therefore, error is thrown. */
    if (!tokenized[0])
    {
        perror("It is not possible for instruction to be empty\n");
        exit(EXIT_FAILURE);
    }

    // Gets meaning of an alias and associated index and type of operation.
    getAlias(tokenized);
    TypePair *tp = getAssembleType(tokenized);
    int32_t result;

    switch (tp->aType)
    {
        case DP_ASS: //Data processing instruction
            result = assembleDPI(tokenized, (DPOperation)(tp->opcode));
            break;
        case BRANCH_ASS: //Branch instruction
            result = assembleBranch(tokenized, (BOperation)(tp->opcode), PC);
            break;
        case SDT_ASS: //Single data transfer instruction
            result = assembleSDT(tokenized, (SDTOperation)(tp->opcode), PC);
            break;
        case SPECIAL_ASS: //Special instruction
            result = assembleSpecial(tokenized, (SPOperation)(tp->opcode));
            break;
        default: //Unhandled instruction
	        fprintf(stderr, "Unhandled assemble type -%s-", tokenized[0]);
            exit(EXIT_FAILURE);
    }

    return result;
}
