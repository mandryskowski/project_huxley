#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "assembleControl.h"
#include "assembleDPI.h"

bool isArithmetic(DPOperation op)
{
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
	switch(op)
	{
		case DP_MADD:
		case DP_MSUB:
			return true;
		default:
			return false;
	}
}

int getRegister(char *c)
{
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

char *getSh(char *c)
{
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

	//Set SF
	setBits(&instruction, (tokenized[1][0] == 'x') ? 1 : 0, 31);

	if(isArithmetic(op))
	{
		//Set destination register and source register
		int rd = getRegister(tail(tokenized[1]));
		int rn = getRegister(tail(tokenized[2]));
		setBits(&instruction, rd, 0); //rd
		setBits(&instruction, rn, 5); //rn

		//Set opcode
		setBits(&instruction, op - FIRST_ARITHMETIC, 29);

		//Set DPI type bitcode
		if(tokenized[3][0] == '#') //Immediate
		{
			setBits(&instruction, 0b100, 26);
			setBits(&instruction, 0b010, 23); //opi
			int imm12 = stoi(tail(tokenized[3]));
			setBits(&instruction, imm12, 10); //imm12

			if(tokenized[4] != NULL && !strcmp(getSh(tail(tokenized[5])), "12")) //shift
			{
				setBits(&instruction, 0b1, 22); //sh
			}

		}
		else //Register
		{
			setBits(&instruction, 0b0101, 25);
			setBits(&instruction, 0b1000, 21); //opr
			int rm = getRegister(tail(tokenized[3]));
			setBits(&instruction, rm, 16); //rm
			if(tokenized[4] != NULL) //shift
			{
				char *shiftType = tokenized[4];
				for(int shiftCode = 0; shiftCode < sizeof(shifts) / sizeof(char *); shiftCode++)
				{

					if(!strcmp(shifts[shiftCode], shiftType))
					{
						setBits(&instruction, shiftCode, 22); //shift
						break;
					}
				}

				int shiftAmount = stoi(tail(tokenized[5]));
				setBits(&instruction, shiftAmount, 10); //operand
			}
		}
	}

	if(isLogical(op))
	{
		//Set destination register and source register
		int rd = getRegister(tail(tokenized[1]));
                int rn = getRegister(tail(tokenized[2]));

		setBits(&instruction, rd, 0); //rd
		setBits(&instruction, rn, 5); //rn
		//Set opcode
		setBits(&instruction, (op - FIRST_LOGICAL) >> 1, 29);

		setBits(&instruction, 0b0101, 25);
		setBits(&instruction, 0b0000, 21); //opr - 0xxx
		int rm = getRegister(tail(tokenized[3]));
		setBits(&instruction, rm, 16); //rm

		setBits(&instruction, ((op-FIRST_LOGICAL) & 1), 21); //N

		if(tokenized[4] != NULL) //shift
		{
			char *shiftType = tokenized[4];
			for(int shiftCode = 0; shiftCode < 4; shiftCode++)
			{
				if(!strcmp(shifts[shiftCode], shiftType))
				{
					setBits(&instruction, shiftCode, 22); //shift
					break;
				}
			}

			int shiftAmount = stoi(tail(tokenized[5]));
			setBits(&instruction, shiftAmount, 10); //operand
		}
	}

	if(isMultiply(op))
	{
		int rd = getRegister(tail(tokenized[1]));
		int rn = getRegister(tail(tokenized[2]));
		int rm = getRegister(tail(tokenized[3]));
		int ra = getRegister(tail(tokenized[4]));

		setBits(&instruction, rd, 0); //rd
		setBits(&instruction, rn, 5); //rn
		setBits(&instruction, ra, 10); //ra
		setBits(&instruction, op - FIRST_MULTIPLY, 15); //x
		setBits(&instruction, rm, 16); //rm
		setBits(&instruction, 0b0011011000, 21);
	}

	if(isWideMove(op))
	{
		int rd = getRegister(tail(tokenized[1]));
		setBits(&instruction, rd, 0);//rd

		setBits(&instruction, 0b100, 26); //imm
		setBits(&instruction, 0b101, 23); //opi
		setBits(&instruction, (int)(op - FIRST_WIDE_MOVE), 29); //opc

		int imm16 = stoi(tail(tokenized[2]));
		setBits(&instruction, imm16, 5);
		if(tokenized[3] != NULL)
		{
			int sh = stoi(tail(tokenized[4])) >> 4;
			setBits(&instruction, sh, 21);
		}
	}

	return instruction;
}
