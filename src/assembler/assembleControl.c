#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "assembleControl.h"
#include "util/assembleUtility.h"
#include "instructions/assembleSpecial.h"
#include "instructions/assembleDPI.h"
#include "instructions/assembleSDT.h"
#include "instructions/assembleBranch.h"
#include "label.h"

#define DELIMETERS " ,"
#define MAX_INSTRUCTION_SIZE 6
#define SPECIAL_REGISTER 31

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

/*Returns a type and index of given operation.*/
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
	        fprintf(stderr, "Unhandled assemble type -%s-", tokenized[0]);
            exit(EXIT_FAILURE);
    }

    free(tokenized);
    return result;
}
