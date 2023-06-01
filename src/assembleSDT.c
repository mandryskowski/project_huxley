#include "assembleSDT.h"
#include "assembleControl.h"
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "control.h"
// bits 5-21 and bit 24
void setSDTOffsetBits(int32_t* word, char* lhsToken, char* rhsToken)
{
	// bits 5-9: xn
	{
		bool endsWithBrace = lhsToken[strlen(lhsToken) - 1] == ']';
		int xn = atoi(substr(lhsToken, 2, strlen(lhsToken) - (endsWithBrace ? 1 : 0)));
		setBits(word, getBits(0, 4, xn), 5);
	}
	// bits 10-21

	if (rhsToken[0] != '#') // register offset	
	{
		setBits(word, 0b011010, 10); // bits 10-15: pattern
		setBits(word, 0b1, 21);	     // bit     21: pattern
		setBits(word, atoi(substr(rhsToken, 1, strlen(rhsToken) - 1)), 16); // bits 16-20: xm 
		return;
	}

	if (rhsToken[strlen(rhsToken) - 1] == ']') // unsigned offset
	{
		setBits(word, getBits(atoi(substr(rhsToken, 1, strlen(rhsToken) - 1)), 0, 11), 10); // bits 10-21: imm12
		setBits(word, 0b1, 24);	// bit 24: set to 1 when it's unsigned offset.
		return;
	}

	// otherwise, pre/post index
	setBits(word, 0b1, 10); // bit 10: pattern
	bool isPreIndexed  = rhsToken[strlen(rhsToken) - 1] == '!'; // if false, it is post indexed.
	setBits(word, isPreIndexed, 11); // bit 11: I (flag indicating that it is pre indexed).
					 //
	int simm9 = atoi(substr(rhsToken, 1, strlen(rhsToken) - (isPreIndexed ? 2 : 1))); // for pre indexed we remove "]!" and for post just "]".
	setBits(word, getBits(0, 8, simm9), 16); // bits 16-20: xm

	return;
}

int32_t assembleSDT(char* str, SDTOperation op, int64_t PC)
{
	char** tokenized = split(str);

	int32_t word = 0;
	
	setBits(&word, 0b1100, 25);	// bits 25-28: 1100 (common pattern for all SDT instructions)
	setBits(&word, atoi(tail(tokenized[1])), 0); // bits 0-4: rt

	switch (op)
	{
		case SDT_LOAD:
		       if (tokenized[2][0] != '[') // Load literal is the only one that doesn't use braces [] to encode.
		       {
			        int offset = (atoi(tail(tokenized[2])) - PC) / 4; 
 				setBits(&word, getBits(0, 18, offset), 5); // bits 5-23 of Load Literal: simm19 (offset from curr PC as a multiple of 4).
				break;
		       }

		       // Otherwise it is a Single Data Transfer.
		       setBits(&word, 0b1, 22); // for Single Data Transfer load bit 22 must be set to 1.
		       setBits(&word, 0b101, 29); // set 31st and 29th bit to 1 for anything other than Load Literal.
			setSDTOffsetBits(&word, tokenized[2], tokenized[3]);

			break;
		case SDT_STORE:
		       setBits(&word, 0b101, 29); // set 31st and 29th bit to 1 for anything other than Load Literal.
			setSDTOffsetBits(&word, tokenized[2], tokenized[3]);

			break;
	}
	setBits(&word, tokenized[1][0] == 'x' ? 1 : 0, 31); // overwrite 31st bit to sf (true if 64 bit false if 32).
	return word;
}
