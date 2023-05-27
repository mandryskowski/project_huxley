#ifndef STATE_H
#define STATE_H

#include <stdbool.h>
#include <stdint.h>

typedef struct ComputerState ComputerState; // It's irritating to use struct every time...

struct Pstate
{
    bool nf, zf, cf, vf;
};

struct ComputerState
{
	int64_t registers[31];
	struct Pstate pstate; // PSTATE flags
	uint64_t stack_ptr;
	uint64_t* PC;
	const int64_t zr; // Zero register
};

// to test a flag use e.g. state->pstate->NF given ComputerState* state.
#endif // STATE_H
