#ifndef LABEL_H
#define LABEL_H
#include <stdint.h>
#include <stddef.h>
#define MAX_UINT64T UINT64_C(-1)
typedef struct Label
{
	char* name;
        int64_t address;
} Label;      

// Returns the address associated with the given labelName.
// You must pass the array containing all known labels.
// If no matching label is found, the function returns MAX_UINT64T..
uint64_t getLabelAddress(char* labelName, Label* labels);
#endif // LABEL_H
