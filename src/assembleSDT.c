#include "assembleSDT.h"
#include "assembleControl.h"
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "control.h"
#include <stdio.h> // for debugging
// bits 5-21 and bit 24
void setSDTOffsetBits(int32_t* word, char* lhsToken, char* rhsToken, bool is64bit)
{
	// bits 5-9: xn
	{
		bool endsWithBrace = lhsToken[strlen(lhsToken) - 1] == ']';
		int xn = atoi(substr(lhsToken, 2, strlen(lhsToken) - (endsWithBrace ? 1 : 0)));
		setBits(word, getBits(0, 4, xn), 5);
	}
	// bits 10-21


	if (rhsToken == NULL || (rhsToken[0] == '#' && rhsToken[strlen(rhsToken) - 1] == ']')) // unsigned offset
	{
		setBits(word, (rhsToken != NULL) ? getBits(0, 11, atoi(substr(rhsToken, 1, strlen(rhsToken) - 1)) / (is64bit ? 8 : 4)) : 0, 10); // bits 10-21: imm12
		setBits(word, 0b1, 24);	// bit 24: set to 1 when it's unsigned offset.
		printf("UO %d\n", rhsToken == NULL ? 0 : (atoi(substr(rhsToken, 1, strlen(rhsToken) - 1))));
		return;
	}

	if (rhsToken[0] != '#') // register offset	
	{
		setBits(word, 0b011010, 10); // bits 10-15: pattern
		setBits(word, 0b1, 21);	     // bit     21: pattern
		setBits(word, atoi(substr(rhsToken, 1, strlen(rhsToken) - 1)), 16); // bits 16-20: xm 
		printf("RO\n");
		return;
	}


	// otherwise, pre/post index
	setBits(word, 0b1, 10); // bit 10: pattern
	bool isPreIndexed  = rhsToken[strlen(rhsToken) - 1] == '!'; // if false, it is post indexed.
	setBits(word, isPreIndexed, 11); // bit 11: I (flag indicating that it is pre indexed).
					 //
	int simm9 = atoi(isPreIndexed ? substr(rhsToken, 1, strlen(rhsToken) - 2) : tail(rhsToken)); // for pre indexed we remove "#" and "]!" and for post just "#". 
	setBits(word, getBits(0, 8, simm9), 12); // bits 12-20: simm9
	printf(isPreIndexed ? ("PRI %d\n") : ("POI %d\n"), simm9);				

	return;
}

int32_t assembleSDT(char** tokenized, SDTOperation op, int64_t PC)
{
	int32_t word = 0;
	const bool is64bit = tokenized[1][0] == 'x';  

	setBits(&word, 0b1100, 25);	// bits 25-28: 1100 (common pattern for all SDT instructions)
	setBits(&word, atoi(tail(tokenized[1])), 0); // bits 0-4: rt
	switch (op)
	{
		case SDT_LOAD:
		       if (tokenized[2][0] != '[') // Load literal is the only one that doesn't use braces [] to encode.
		       {
			        int offset = (atoi(tail(tokenized[2])) - PC) / 4; 
 				setBits(&word, getBits(0, 18, offset), 5); // bits 5-23 of Load Literal: simm19 (offset from curr PC as a multiple of 4).
				printf("LL\n");break;
		       }

		       // Otherwise it is a Single Data Transfer.
		       setBits(&word, 0b1, 22); // for Single Data Transfer load bit 22 must be set to 1.
		       setBits(&word, 0b101, 29); // set 31st and 29th bit to 1 for anything other than Load Literal.
			setSDTOffsetBits(&word, tokenized[2], tokenized[3], is64bit);

			break;
		case SDT_STORE:
		       setBits(&word, 0b101, 29); // set 31st and 29th bit to 1 for anything other than Load Literal.
			setSDTOffsetBits(&word, tokenized[2], tokenized[3], is64bit);

			break;
	}
	setBits(&word, is64bit, 30); // overwrite bit 30 to sf (true if 64 bit false if 32).
	return word;
}
