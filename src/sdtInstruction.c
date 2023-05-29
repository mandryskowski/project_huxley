#include "sdtInstruction.h"
#include "state.h"

int64_t loadFromMemory(const char* absoluteAddress, bool is64bit)
{
	return is64bit ? (*(int32_t*)absoluteAddress) : (*(int64_t*)absoluteAddress);
}



void sdtInstruction(ComputerState* state, const int64_t instruction,
		      bool is64bit, const int rt)
{
	const bool L = readBit(22, instruction);
    const bool U = readBit(24, instruction);

	const int offset = readBits(10, 21, instruction);
	const int xn = readBits(5, 9, instruction);

	int64_t address = xn; // transfer address

	if (U) // Unsigned offset	
	{
		const int imm12 = readBits(10, 21, instruction);                                         
		address += imm12;

		if (is64bit)
			assert (imm12 <= (2 << 15) && imm12 % 8 == 0); // in 64-bit, 0 <= imm12 <= 2^15 and imm12 is a multiple of 8.
		else
			assert (imm12 <= (2 << 14) && imm12 % 4 == 0); // in 32-bit, 0 <= imm12 <= 2^14 and imm12 is a multiple of 4.

	}

	else if (!readBit(21, instruction)
	       && readBit(10, instruction)) // Pre/Post-Index
	{
		const bool I = readBit(11, instruction);
		const int simm9 = readBitsSignExt(12, 20, instruction);		
		assert(simm9 >= -256 && simm9 <= 255);
		if (I)
			address += simm9;

		state->registers[xn] += simm9;		
	}
	else // Register offset
	{
		const int xm = readBits(16, 20, instruction);
		address += xm;
	}

	const char* absoluteAddress = state->memory + address;

	if (L)
	{
		state->registers[rt] = loadFromMemory(absoluteAddress, is64bit);
	}
	else
	{
		if (is64bit)
			*(int64_t*)(absoluteAddress) = state->registers[rt];
		else
			*(int32_t*)(absoluteAddress) = state->registers[rt];	
	}

}


void loadLiteralInstruction(ComputerState *state, const int64_t instruction, bool is64bit, const int rt)
{
	const int32_t simm19 = readBitsSignExt(5, 23, instruction);
	state->registers[rt] = loadFromMemory(state->memory + state->PC + simm19 * 4, is64bit);
}

void executeSdtInstruction(ComputerState* state, const int64_t instruction)
{
	const bool sf = readBit(30, instruction);
	const bool op = readBit(29, instruction);
	const int rt = readBits(0, 4, instruction);


	if (op) // Single Data Transfer
	{
		sdtInstruction(state, instruction, sf, rt);
	}
	else // Load Literal
	{
		loadLiteralInstruction(state, instruction, sf, rt);
	}
}

void test()
{
	char* mem = malloc(256);
	int64_t* memll = mem + 4;
	memll = 0x0000000100000001;


	printf("0x%x", loadFromMemory(mem + 4, 1));
	printf("0x%x", loadFromMemory(mem + 4, 0));


}

int main()
{
	test();
	return 0;
}