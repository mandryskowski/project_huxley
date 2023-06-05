#include <string.h>
#include <stdbool.h>
#include "assembleControl.h"
#include "assembleDPI.h"

bool isArithmetic(DPOperation op)
{
	//Checks if DPOperation is an arithmetic op
	switch (op)
	{
		case DP_ADD:
		case DP_ADDS:
		case DP_SUB:
		case DP_SUBS:
			return true;
		default:
			return false;
	}
}

bool isLogical(DPOperation op)
{
	//Checks if DPOperation is an logical op
	switch (op)
	{
		case DP_AND:
		case DP_ANDS:
		case DP_BIC:
		case DP_BICS:
		case DP_EOR:
		case DP_EON:
		case DP_ORR:
		case DP_ORN:
			return true;
		default:
			return false;
	}
}

bool isWideMove(DPOperation op)
{
	//Checks if DPOperation is an wide move op
	switch(op)
	{
		case DP_MOVN:
		case DP_MOVZ:
		case DP_MOVK:
			return true;
		default:
			return false;
	}
}

bool isMultiply(DPOperation op)
{
	//Checks if DPOperation is an multiply op
	switch(op)
	{
		case DP_MADD:
		case DP_MSUB:
			return true;
		default:
			return false;
	}
}

char *getSh(char *c)
{
	//Gets the first (at most) 2 characters of c
	if(strlen(c) < 2)
	{
		return c;
	}

	return substr(c, 0, 2);
}

int32_t assembleDPI(char **tokenized, DPOperation op)
{

	int32_t instruction = 0;

	char* shifts[] = {"lsl", "lsr", "asr", "ror"};

	setBits(&instruction, tokenized[1][0] == 'x', 31); //Set sf on bit 31

	if(isArithmetic(op))
	{
		//It it an arithmetic operation

		//Get destination register and source register
		int rd = getRegister(tokenized[1]);
		int rn = getRegister(tokenized[2]);
		setBits(&instruction, rd, 0); //Set rd on bits 0-4
		setBits(&instruction, rn, 5); //Set rn on bits 5-9

		//Set opcode on bits 29-30
		setBits(&instruction, op - FIRST_ARITHMETIC, 29);

		if(tokenized[3][0] == '#')
		{
			//If it begins with #, it is an immediate value and thus a immediate instruction
			setBits(&instruction, 0b100, 26); //Set immediate mask on bits 26-28
			setBits(&instruction, 0b010, 23); //Set opi on bits 23-25

			int imm12 = getImmediate(tokenized[3]); //Get immediate value
			setBits(&instruction, imm12, 10); //Set imm12 on bits 10-21

			if(tokenized[4] != NULL && !strcmp(getSh(tail(tokenized[5])), "12")) //shift
			{
				//If the 4th arg isn't NULL, there exists a shift instruction
				//If the immediate value which follows the shift is #12, then it has to be
				//logically left shifted by 1
				setBits(&instruction, 0b1, 22); //Set sh on bit 22
			}

		}
		else
		{
			//Otherwise, it is a register
			setBits(&instruction, 0b0101, 25); //Set M (0 because it's not multiply) and register mask on bits 26-29
			setBits(&instruction, 0b1000, 21); //Set opr on bits 21-25

			int rm = getRegister(tokenized[3]); //Get rm register index
			setBits(&instruction, rm, 16); //Set rm on bits 16-20

			if(tokenized[4] != NULL)
			{
				//If the 4th arg isn't NULL, there exists a shift instruction
				char *shiftType = tokenized[4]; // Get shift type
				for(int shiftCode = 0; shiftCode < sizeof(shifts) / sizeof(char *); shiftCode++)
				{

					if(!strcmp(shifts[shiftCode], shiftType))
					{
						setBits(&instruction, shiftCode, 22); //Set shift on bits 22-23
						break;
					}
				}

				int shiftAmount = getImmediate(tokenized[5]); //Get shift amount
				setBits(&instruction, shiftAmount, 10); //Set operand(sh amm) on bits 10-15
			}
		}
	}

	if(isLogical(op))
	{
		//Get all registers
		int rd = getRegister(tokenized[1]);
                int rn = getRegister(tokenized[2]);
		int rm = getRegister(tokenized[3]);

		setBits(&instruction, rd, 0); //Set rd on bits 0-4
		setBits(&instruction, rn, 5); //Set rn on bits 5-9
		setBits(&instruction, rm, 16); //Set rm on bits 16-20

		setBits(&instruction, (op - FIRST_LOGICAL) >> 1, 29); //Set opcode on bits 29-30

		setBits(&instruction, 0b0101, 25); //Set bit-logic opcode on bits 25-28
		setBits(&instruction, 0b0000, 21); //Set opr mask (0xxx) on bits 21-24

		setBits(&instruction, ((op-FIRST_LOGICAL) & 1), 21); //Set N on bit 21

		if(tokenized[4] != NULL)
		{
			//If the 4th arg isn't NULL, there exists a shift instruction
			char *shiftType = tokenized[4]; //Get shiftTypehttps://www.rightmove.co.uk/properties/135517445#/floorplan?activePlan=1&channel=RES_LET

			for(int shiftCode = 0; shiftCode < 4; shiftCode++)
			{
				if(!strcmp(shifts[shiftCode], shiftType))
				{
					setBits(&instruction, shiftCode, 22); //Set shiftCode on bits 22-23
					break;
				}
			}

			int shiftAmount = getImmediate(tokenized[5]); // Get shift amount
			setBits(&instruction, shiftAmount, 10); //Set operand on bits 10-15
		}
	}

	if(isMultiply(op))
	{
		//It's a multiply operation
		//Get all registers
		int rd = getRegister(tokenized[1]);
		int rn = getRegister(tokenized[2]);
		int rm = getRegister(tokenized[3]);
		int ra = getRegister(tokenized[4]);

		setBits(&instruction, rd, 0); //Set rd on bits 0-4
		setBits(&instruction, rn, 5); //Set rn on bits 5-9
		setBits(&instruction, ra, 10); //Set ra on bits 10-14
		setBits(&instruction, op - FIRST_MULTIPLY, 15); //Set x (mul type) on bit 15
		setBits(&instruction, rm, 16); //Set rm on bits 16-20
		setBits(&instruction, 0b0011011000, 21); //Set multiply opcode on bits 21-31
	}

	if(isWideMove(op))
	{
		//It's a wide move operation

		int rd = getRegister(tokenized[1]); //Get rd register
		setBits(&instruction, rd, 0); //Set rd on bits 0-4

		setBits(&instruction, (int)(op - FIRST_WIDE_MOVE), 29); //Set opc on bits 29-30
		setBits(&instruction, 0b100, 26); //Set immediate operation on bits 26-28
		setBits(&instruction, 0b101, 23); //Set opi on bits 23-25

		int imm16 = getImmediate(tokenized[2]); //Get imm16
		setBits(&instruction, imm16, 5); //Set imm16 on bits 5-20
		if(tokenized[3] != NULL)
		{
			//If 3rd argument is NULL, there is a shift on the wide move
			int sh = getImmediate(tokenized[4]) >> 4;
			setBits(&instruction, sh, 21);
		}
	}

	return instruction;
}
