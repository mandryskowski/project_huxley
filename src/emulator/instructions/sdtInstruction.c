#include "sdtInstruction.h"
#include "../util/state.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "../util/emulateUtility.h"
#include <inttypes.h>

int64_t loadFromMemory(const char* absoluteAddress, bool is64bit)
{
	return is64bit ? (*(int64_t*)absoluteAddress) : (*(int32_t*)absoluteAddress);
}

int64_t* getRegisterOrSP(ComputerState* state, int reg)
{
	return (reg == 0x1F /* which is 0b11111 */) ? ((int64_t*)&state->stack_ptr) : (&state->registers[reg]);
}

void sdtInstruction(ComputerState* state, const int64_t instruction,
		      bool is64bit, const int rt)
{
	const bool L = getBit(22, instruction);
    const bool U = getBit(24, instruction);
	const int xn = getBits(5, 9, instruction);

	int64_t address = *getRegisterOrSP(state, xn); // transfer address

	if (U) // Unsigned offset
	{
		const int imm12 = getBits(10, 21, instruction);
		address += imm12 * ((is64bit) ? (8) : (4)); // imm12 encodes offset as a multiple of 8/4 in 64/32bit.
	}

	else if (!getBit(21, instruction)
	       && getBit(10, instruction)) // Pre/Post-Index
	{
		const bool I = getBit(11, instruction);
		const int simm9 = getBitsSignExt(12, 20, instruction);
		
		if (I) // if pre-index
		{
			address += simm9;
		}

		*getRegisterOrSP(state, xn) += simm9;
	}

	else // Register offset
	{
		assert(getBit(21, instruction) && getBits(10, 15, instruction) == 0x1A); // verify the rest of pattern (0b011010)
		const int xm = getBits(16, 20, instruction);
		address += *getRegisterOrSP(state, xm);
	}

	const char* absoluteAddress = state->memory + address;

	if (L)
	{
		state->registers[rt] = loadFromMemory(absoluteAddress, is64bit);
	}
	else
	{
		if (is64bit)
		{
			*(int64_t*)(absoluteAddress) = state->registers[rt];
		}
		else
		{
			*(int32_t*)(absoluteAddress) = state->registers[rt];
		}
	}

}


void loadLiteralInstruction(ComputerState *state, const int64_t instruction, bool is64bit, const int rt)
{
	const int32_t simm19 = getBitsSignExt(5, 23, instruction);
	state->registers[rt] = loadFromMemory(state->memory + state->PC + simm19 * 4, is64bit); // simm19 encodes offset as a multiple of 4
}

void ExecuteSdtInstruction(const int64_t instruction, ComputerState* state)
{
	// Common pattern
	const bool sf = getBit(30, instruction);
	const bool op = getBit(29, instruction);
	const int rt = getBits(0, 4, instruction);

	// Decide based on op
	if (op) // Single Data Transfer
	{
		sdtInstruction(state, instruction, sf, rt);
	}
	else // Load Literal
	{
		loadLiteralInstruction(state, instruction, sf, rt);
	}
}