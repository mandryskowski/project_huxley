#include "assembleSDT.h"
#include "../util/assembleUtility.h"
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

// bits 5-21 and bit 24
void setSDTOffsetBits(int32_t* word, char* lhsToken, char* rhsToken, bool is64bit)
{
	// bits 5-9: xn
	{
		bool endsWithBrace = lhsToken[strlen(lhsToken) - 1] == ']';
        char *registerString = substr(lhsToken, 1, strlen(lhsToken) - (endsWithBrace ? 1 : 0));
		int xn = getRegister(registerString); // from lhs, remove "[" and "]" if it exists
		setBits(word, truncateBits(xn, 5), 5);
        free(registerString);
	}

	// bits 10-21

	if (rhsToken == NULL || (rhsToken[0] == '#' && rhsToken[strlen(rhsToken) - 1] == ']')) // unsigned offset
	{
        if(rhsToken != NULL) {
            rhsToken[strlen(rhsToken) - 1] = '\0';
            setBits(word,  truncateBits(getImmediate(rhsToken) / (is64bit ? 8 : 4), 12),
                    10); // bits 10-21: imm12 (remove "]")
        }
        setBits(word, 0x1, 24);	// bit 24: set to 1 when it's unsigned offset.
		return;
	}

	if (rhsToken[0] != '#') // register offset	
	{
		setBits(word, 0x1A, 10); 	 // bits 10-15: pattern (mask 0b011010)
		setBits(word, 0x1, 21);	     // bit     21: pattern
        rhsToken[strlen(rhsToken) - 1] = '\0';
		setBits(word, getRegister(rhsToken), 16); // bits 16-20: xm (remove "]").
		return;
	}


	// otherwise, pre/post index
	setBits(word, 0x1, 10); // bit 10: pattern
	bool isPreIndexed  = rhsToken[strlen(rhsToken) - 1] == '!'; // if false, it is post indexed.
	setBits(word, isPreIndexed, 11); // bit 11: I (flag indicating that it is pre indexed).
    int simm9;
    if(isPreIndexed)
    {
        char *preIndexImm = substr(rhsToken, 0, strlen(rhsToken) - 2);
        simm9 = getImmediate(preIndexImm);
        free(preIndexImm);
    }
    else
    {
        simm9 = getImmediate(rhsToken); // for pre indexed we remove "#" and "]!" and for post just "#".
    }
	setBits(word, truncateBits(simm9, 9), 12); // bits 12-20: simm9
}

int32_t assembleSDT(char** tokenized, SDTOperation op, int64_t PC)
{
	int32_t word = 0;
	const bool is64bit = tokenized[1][0] == 'x';  

	setBits(&word, 0xC, 25);	// bits 25-28: 0b1100 (common pattern for all SDT instructions)
	setBits(&word, getRegister(tokenized[1]), 0); // bits 0-4: rt
	switch (op)
	{
		case SDT_LOAD:
			if (tokenized[2][0] != '[') // Load literal is the only one that doesn't use braces [] to encode.
		    {
				int offset = (getImmediate(tokenized[2]) - PC) / 4; 
 				setBits(&word, truncateBits(offset, 19), 5); // bits 5-23 of Load Literal: simm19 (offset from curr PC as a multiple of 4).
				break;
		    }

		    // Otherwise it is a Single Data Transfer.
		    setBits(&word, 0x1, 22); // for Single Data Transfer load bit 22 must be set to 1.
		    setBits(&word, 0x5, 29); // set 31st and 29th bit to 1 for anything other than Load Literal. (mask 0b101)
			setSDTOffsetBits(&word, tokenized[2], tokenized[3], is64bit);

			break;
		case SDT_STORE:
		    setBits(&word, 0x5, 29); // set 31st and 29th bit to 1 for anything other than Load Literal. (mask 0b101)
			setSDTOffsetBits(&word, tokenized[2], tokenized[3], is64bit);

			break;
	}
	setBits(&word, is64bit, 30); // overwrite bit 30 to sf (true if 64 bit false if 32).
	return word;
}
