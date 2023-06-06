#include <string.h>
#include <stdbool.h>
#include "../util/assembleUtility.h"
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

int32_t assembleDPI(char **tokenized, DPOperation op)
{

	int32_t instruction = 0;
	//Set SF
	setBits(&instruction, tokenized[1][0] == 'x', 31);

	if(isArithmetic(op))
	{
		// Get destination register and source register
		int rd = getRegister(tokenized[1]);
		int rn = getRegister(tokenized[2]);
		setBits(&instruction, rd, 0); // Set rd on bits 0-4
		setBits(&instruction, rn, 5); // Set rn on bits 5-9

		//Set opcode on bits 29-30
		setBits(&instruction, op - FIRST_ARITHMETIC, 29);

		if(tokenized[3][0] == '#') // Immediate value
		{
			setBits(&instruction, 0x4, 26); // Set immediate mask on bits 26-28 (mask 0b100)
			setBits(&instruction, 0x2, 23); // Set opi on bits 23-25 (mask 0b010)
			int imm12 = getImmediate(tokenized[3]); // Get immediate imm12
			setBits(&instruction, imm12, 10); // Set imm12 on bits 10-21

			if(tokenized[4] != NULL && getImmediate(tokenized[5]) == 12) //shift
			{
				/* If the 4th arg isn't NULL, there exists a shift instruction
				*  If the immediate value which follows the shift is #12, then it has to be
				*  logically left shifted by 1 */

				setBits(&instruction, 0x1, 22); // Set sh on bit 22 (mask 0b1)
			}

		}
		else //Register
		{
			setBits(&instruction, 0x5, 25); // Set M (0 because it's not multiply) and register mask on bits 26-29 (so resulting mask is 0b0101))
			setBits(&instruction, 0x8, 21); // Set opr on bits 21-25 (mask 0b1000)

			int rm = getRegister(tokenized[3]); // Get rm register index
			setBits(&instruction, rm, 16); // Set rm on bits 16-20
			if(tokenized[4] != NULL) //shift
			{
				//If the 4th arg isn't NULL, there exists a shift instruction
				char *shiftType = tokenized[4]; // Get shift type
				setBits(&instruction, getShiftCode(shiftType), 22); //Set shift on bits 22-23

				int shiftAmount = getImmediate(tokenized[5]); //Get shift amount
				setBits(&instruction, shiftAmount, 10); //Set operand(sh amm) on bits 10-15
			}
		}
	}

	if(isLogical(op))
	{
		// Get all registers
		int rd = getRegister(tokenized[1]);
        int rn = getRegister(tokenized[2]);
		int rm = getRegister(tokenized[3]);

		setBits(&instruction, rd, 0); // Set rd on bits 0-4
		setBits(&instruction, rn, 5); // Set rn on bits 5-9
		setBits(&instruction, rm, 16); // Set rm on bits 16-20

		setBits(&instruction, (op - FIRST_LOGICAL) >> 1, 29); // Set opcode on bits 29-30

		setBits(&instruction, 0x5, 25); // Set bit-logic opcode on bits 25-28 (mask 0b0101)
		setBits(&instruction, 0x0, 21); // Set opr mask (0xxx) on bits 21-24
		

		setBits(&instruction, ((op-FIRST_LOGICAL) & 1), 21); // Set N on bit 21

		if(tokenized[4] != NULL) // Shift
		{
			char *shiftType = tokenized[4]; // Get shiftType
			setBits(&instruction, getShiftCode(shiftType), 22); // Set shift on bits 22-23
			
			int shiftAmount = getImmediate(tokenized[5]); // Get shift amount
			setBits(&instruction, shiftAmount, 10); // Set operand on bits 10-15
		}
	}

	if(isMultiply(op))
	{
		//Get all registers
		int rd = getRegister(tokenized[1]);
		int rn = getRegister(tokenized[2]);
		int rm = getRegister(tokenized[3]);
		int ra = getRegister(tokenized[4]);

		setBits(&instruction, rd, 0); // Set rd on bits 0-4
		setBits(&instruction, rn, 5); // Set rn on bits 5-9
		setBits(&instruction, ra, 10); // Set ra on bits 10-14
		setBits(&instruction, op - FIRST_MULTIPLY, 15); // Set x (mul type) on bit 15
		setBits(&instruction, rm, 16); // Set rm on bits 16-20
		setBits(&instruction, 0xD8, 21); // Set multiply opcode on bits 21-310 (mask 0b0011011000)
	}

	if(isWideMove(op))
	{
		int rd = getRegister(tokenized[1]); // Get rd register
		setBits(&instruction, rd, 0); // Set rd on bits 0-4

		setBits(&instruction, (int)(op - FIRST_WIDE_MOVE), 29); // Set opc on bits 29-30
		setBits(&instruction, 0x4, 26); // Set immediate operation on bits 26-28 (mask 0b100)
		setBits(&instruction, 0x5, 23); // Set opi on bits 23-25 (mask 0b101)

		int imm16 = getImmediate(tokenized[2]); // Get imm16
		setBits(&instruction, imm16, 5); // Set imm16 on bits 5-20
		
		if(tokenized[3] != NULL) // Shift on wide move
		{
			int sh = getImmediate(tokenized[4]) >> 4;
			setBits(&instruction, sh, 21);
		}
	}

	return instruction;
}
