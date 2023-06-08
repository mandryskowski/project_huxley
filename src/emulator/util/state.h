#ifndef ARMV8_STATE_H
#define ARMV8_STATE_H

#include <stdbool.h>
#include <stdint.h>

#define ZR 0 // Zero register
#define MEMORY_SIZE (1<<21) //Size of memory

struct Pstate
{
    bool nf, zf, cf, vf;
};

typedef struct ComputerState
{
	int64_t registers[31];
	struct Pstate pstate; // PSTATE flags
	uint64_t stack_ptr;
	uint64_t PC;
	char* memory;
} ComputerState;

// to test a flag use e.g. state->pstate->NF given ComputerState* state.
#endif // ARMV8_STATE_H
