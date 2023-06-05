#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "../util/emulateUtility.h"
#include "sdtInstruction.h"

int64_t loadFromMemory(const char* absoluteAddress, bool is64bit)
{
	return is64bit ? (*(int64_t*)absoluteAddress) : (*(int32_t*)absoluteAddress);
}

int64_t* getRegisterOrSP(ComputerState* state, int reg)
{
	return (reg == 0b11111) ? ((int64_t*)&state->stack_ptr) : (&state->registers[reg]);
}

void sdtInstruction(ComputerState* state, const int64_t instruction,
		      bool is64bit, const int rt)
{
	const bool L = getBit(22, instruction);
    const bool U = getBit(24, instruction);

	//const int offset = getBits(10, 21, instruction); -unnecessary
	const int xn = getBits(5, 9, instruction);

	int64_t address = *getRegisterOrSP(state, xn); // transfer address

	if (U) // Unsigned offset
	{
		const int imm12 = getBits(10, 21, instruction);
		address += imm12 * ((is64bit) ? (8) : (4));
	}

	else if (!getBit(21, instruction)
	       && getBit(10, instruction)) // Pre/Post-Index
	{
		const bool I = getBit(11, instruction);
		const int simm9 = getBitsSignExt(12, 20, instruction);
		assert(simm9 >= -256 && simm9 <= 255);
		if (I)
			address += simm9;

		*getRegisterOrSP(state, xn) += simm9;
	}

	else // Register offset
	{
		const int xm = getBits(16, 20, instruction);
		address += *getRegisterOrSP(state, xm);
	}

	printf("address is %ld \n", address);
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
	state->registers[rt] = loadFromMemory(state->memory + state->PC + simm19 * 4, is64bit);
}

void ExecuteSdtInstruction(const int64_t instruction, ComputerState* state)
{
	const bool sf = getBit(30, instruction);
	const bool op = getBit(29, instruction);
	const int rt = getBits(0, 4, instruction);
	if (op) // Single Data Transfer
	{
		sdtInstruction(state, instruction, sf, rt);
	}
	else // Load Literal
	{
		loadLiteralInstruction(state, instruction, sf, rt);
	}
}

/*
========================
==========TEST==========
========================
*/

char* sdtiTest_exampleMemory()
{
	char* mem = malloc(256);
	int64_t* memll = (int64_t*)(mem + 4);
	*memll = 0x0abcdef100000001;
	return mem;
}

void sdtiTest_loadFromMemory()
{
	char *mem = sdtiTest_exampleMemory();
	assert(loadFromMemory(mem + 4, false) == 0x1);
	assert(loadFromMemory(mem + 4, true) == 0xabcdef100000001);
}

int truncBits(int target, int maxBits)
{
	return getBits(0, maxBits - 1, target);
}

/*
int32_t sdtiTest_genSDTInstruction(bool sf, bool U, bool L, int offset, int xn, int rt)
{
	rt = truncBits(rt, 5);
	xn = truncBits(xn, 5) << 5;
	offset = truncBits(offset, 12) << 10;

	return rt | xn | offset | (L << 22) | (U << 24) | (0b10111 << 27) | (sf << 30);
}

int32_t sdtiTest_genLLInstruction(bool sf, int simm19, int rt)
{
	rt = truncBits(rt, 5);
	simm19 = truncBits(simm19, 19) << 5;

	return rt | simm19 | (0b11 << 27) | (sf << 30);
}

void sdtiTest_loadLiteralInstruction()
{
	ComputerState *state = calloc(1, sizeof(ComputerState));
	state->memory = malloc(MEMORY_SIZE);
	memset(state->memory, 0x00, MEMORY_SIZE);
	memset(state->memory + 120, 0x01, 8);

	// Load 32-bit value
	assert(state->registers[30] == 0);
	state->PC = 128;
	loadLiteralInstruction(state, sdtiTest_genLLInstruction(false, -2, 30), false, 30);
	assert(state->registers[30] == 0x01010101);

	// Load 64-bit value
	assert(state->registers[5] == 0);
	state->PC = 120;
	loadLiteralInstruction(state, sdtiTest_genLLInstruction(true, 0, 5), true, 5);
	assert(state->registers[5] == 0x0101010101010101);

	// Test smallest possible simm19 (-2^18) on 32-bit.
	assert(state->registers[0] == 0);
	state->PC = 120 + 4 * (1 << 18);
	loadLiteralInstruction(state, sdtiTest_genLLInstruction(false, -(1 << 18), 0), false, 0);
	assert(state->registers[0] == 0x01010101);

	// Test largest possible simm19 (2^18 - 1) on 64-bit.
	assert(state->registers[2] == 0);
	memset(state->memory + 4 * ((1 << 18) - 1), 0x5b, 8);
	state->PC = 0;
	loadLiteralInstruction(state, sdtiTest_genLLInstruction(true, (1 << 18) - 1, 2), true, 2);
	assert(state->registers[2] == 0x5b5b5b5b5b5b5b5b);

	free(state);
}
void sdtiTest_executeSdtInstruction()
{
	char* memory = malloc(MEMORY_SIZE);
	memset(memory, 0, MEMORY_SIZE);

	// 32-bit unsigned offset store
	{
		ComputerState *state = calloc(1, sizeof(ComputerState));
		state->memory = memory;
		state->registers[23] = 0x1234;
		state->registers[25] = 0x942424242;
		ExecuteSdtInstruction(state, sdtiTest_genSDTInstruction(false, true, false, (1 << 12) - 1, 23, 25));
		assert(*(int32_t*)(state->memory + 0x1234 + (1 << 14) - 4) == 0x42424242);
		free(state);
	}

	// 32-bit unsigned offset load
	{
		ComputerState *state = calloc(1, sizeof(ComputerState));
		state->memory = memory;
		state->registers[21] = 0x4321;
		*(int64_t*)(state->memory + (1 << 6) + 0x4321) = 0xabcdef76543210;
		ExecuteSdtInstruction(state, sdtiTest_genSDTInstruction(false, true, true, (1 << 4), 21, 22));
		assert(state->registers[22] == 0x76543210);
		free(state);
	}

	// 64-bit unsigned offset store
	{
		ComputerState *state = calloc(1, sizeof(ComputerState));
		state->memory = memory;
		state->registers[15] = 0x2857;
		state->registers[16] = 0x2929292956565656;
		ExecuteSdtInstruction(state, sdtiTest_genSDTInstruction(true, true, false, (1 << 5), 15, 16));
		assert(*(int64_t*)(state->memory + 0x2857 + (1 << 8)) == 0x2929292956565656);
		free(state);
	}

	// 64-bit unsigned offset load
	{
		ComputerState *state = calloc(1, sizeof(ComputerState));
		state->memory = memory;
		state->registers[11] = 0x0cde;
		*(int64_t*)(state->memory + (1 << 15) - 8 + 0x0cde) = 0xfdecba98765432;
		ExecuteSdtInstruction(state, sdtiTest_genSDTInstruction(true, true, true, (1 << 12) - 1, 11, 12));
		assert(state->registers[12] == 0xfdecba98765432);
		free(state);
	}

	//////////////////

	// Pre-Indexed load
	{
	}
}

void sdtiTestSuite()
{
	sdtiTest_loadFromMemory();
	printf("Load from memory OK\n");
	sdtiTest_loadLiteralInstruction();
	printf("Load literal OK\n");
	sdtiTest_executeSdtInstruction();
	printf("Single Data Transfer OK\n");
}

int main()
{
	sdtiTestSuite();
	return 0;
}*/
