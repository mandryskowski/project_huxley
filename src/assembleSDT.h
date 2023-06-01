#ifndef ASSEMBLE_SDT_H
#define ASSEMBLE_SDT_H
#include <inttypes.h>

typedef enum
{
	SDT_LOAD,
	SDT_STORE
} SDTOperation;
int32_t assembleSDT(char* str, SDTOperation, int64_t PC);

#endif // ASSEMBLE_SDT_H
