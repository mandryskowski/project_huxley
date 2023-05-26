#ifndef STATE_H
#define STATE_H

struct ComputerState
{
	int registers[31];
	Pstate pstate; // PSTATE flags	
	uint64_t* stack_ptr;
	uint64_t* PC;
	const uint64_t zr = 0; // Zero register
};

struct Pstate
{
	bool nf, zf, cf, vf;
};
// to test a flag use e.g. state->pstate->NF given ComputerState* state.
#endif STATE_H
