#ifndef ASSEMBLEDPI_H
#define ASSEMBLEDPI_H

typedef enum
{
	DP_ADD,
	DP_ADDS,
	DP_SUB,
	DP_SUBS,
	DP_AND,
	DP_ANDS,
	DP_BIC,
	DP_BICS,
	DP_EOR,
	DP_EON,
	DP_ORR,
	DP_ORN,
	DP_MOVN,
	DP_MOV_UNDEFINED,
	DP_MOVK,
	DP_MOVZ,
	DP_MADD,
	DP_MSUB
} DPOperation;

int32_t assembleDPI(char* str, DPOperation);

#endif //ASSEMBLEDPI_H
