#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "assembleControl.h"

int getRegister(char *c)
{
	return (strcmp(c, "zr")) ? atoi(c), 0b11111;
}

int32_t assembleDPI(char *c)
{
	//PRE: c is all lowercase, Michal please dont forger

	char** tokenized = split(c, DELIMITERS);
	int32_t instruction = 0;

	char* shifts[4] = ["lsl", "lsr", "asr", "ror"];
	char* artm[4] = ["add", "adds", "sub", "subs"];
	char
	//Set SF
	setBits(instruction, (tokenized[1][0] == 'x'), 31);

	int artmIndex = -1;
	for(int i = 0; i < 4; i++) // Check if is arithmetic instruction
	{
		if(!strcmp(artm[i], tokenized[0])
		{
			artmIndex = i;
			break;
		}
	}

	if(artmIndex != -1)
	{
		//Set destination register and source register
		int rd = getRegister(substr(tokenized[1], 1, strlen(tokenized[1])));
                int rn = getRegister(substr(tokenized[2], 1, strlen(tokenized[2])));

		setBits(&instruction, rd, 0); //rd
		setBits(&instruction, rn, 5); //rn

		//Set opcode
		setBits(&instruction, artmIndex, 29);

		//Set DPI type bitcode
		if(tokenized[3][0] == '#') //Immediate
		{
			setBits(&instruction, 0b100, 26);
			setBits(&instruction, 0b010, 23); //opi
			int imm12 = atoi(substr(tokenized[3], 1, strlen(tokenized[3]));
			setBits(&instruction, imm12, 10); //imm12

			if(strlen(tokenized) > 3 && !strcmp(tokenized[4], "#12")) //shift
			{
				setBits(&instruction, 0b1, 22); //sh
			}

		}
		else //Register
		{
			setBits(&instruction, 0b0101, 25);
			setBits(&instruction, 0b1000, 21); //opr
			int rm = getRegister(substr(tokenized[3], 1, strlen(tokenized[3]));
			setBits(&instruction, rm, 16); //rm

			if(strlen(tokenized) > 4) //shift
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

				int shiftAmount = atoi(substr(tokenized[5], 1, strlen(tokenized[5])));
				setBits(&instruction, shiftAmount, 10); //operand
			}
		}
	}

	char* logic[8] = ["and", "bic", "orr", "orn", "eor", "eon", "ands", "bics"];
	int logicIndex = -1;

	for(int i = 0; i < 8; i++)
	{
		if(!strcmp(logic[i], tokenized[0]))
		{
			logicIndex = i;
			break;
		}
	}

	if(logicIndex != -1)
	{
		//Set destination register and source register
		int rd = getRegister(substr(tokenized[1], 1, strlen(tokenized[1])));
                int rn = getRegister(substr(tokenized[2], 1, strlen(tokenized[2])));

		setBits(&instruction, rd, 0); //rd
		setBits(&instruction, rn, 5); //rn

		//Set opcode
		setBits(&instruction, logicIndex >> 1, 29);

		setBits(&instruction, 0b0101, 25);
		setBits(&instruction, 0b0000, 21); //opr - 0xxx
		int rm = getRegister(substr(tokenized[3], 1, strlen(tokenized[3]));
		setBits(&instruction, rm, 16); //rm

		setBits(&instruction, ((logicIndex & 1) == 1), 21); //N

		if(strlen(tokenized) > 4) //shift
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

			int shiftAmount = atoi(substr(tokenized[5], 1, strlen(tokenized[5])));
			setBits(&instruction, shiftAmount, 10); //operand
		}
	}

	if(!strcmp(tokenized[0], "madd") || !strcmp(tokenized[0], "msub"))
	{
		int rd = getRegister(substr(tokenized[1], 1, strlen(tokenized[1])));
                int rn = getRegister(substr(tokenized[2], 1, strlen(tokenized[2])));
		int rd = getRegister(substr(tokenized[3], 1, strlen(tokenized[3])));
                int rn = getRegister(substr(tokenized[4], 1, strlen(tokenized[4])));
		setBits(&instruction, rd, 0); //rd
		setBits(&instruction, rn, 5); //rn
		setBits(&instruction, ra, 10); //ra
		setBits(&instruction, (strcmp(tokenized[0], "msub") == 0), 15); //x
		setBits(&instruction, rm, 16); //rm
		setBits(&instruction, 0b0011011000, 21);
	}

	char* wMoves[3] = ["movn", "movk", "movn"];
	int movIndex = -1;

	for(int i = 0; i < 3; i++)
	{
		if(!strcmp(tokenized[0], wMoves[i]))
		{
			movIndex = i;
			break;
		}
	}

	if(movIndex != -1)
	{
		setBits(&instruction, 0b100, 26); //imm
		setBits(&instruction, 0b101, 23); //opi
		setBits(&instruction, movIndex, 29); //opc

		int imm16 = atoi(substr(tokenized[1], 1, strlen(tokenized[1])));
		int sh = atoi(substr(tokenized[3], 1, strlen(tokenized[3])));
		setBits(&instruction, sh, 21);
		setBits(&instruction, imm16, 5);
	}

	return instruction;
}
