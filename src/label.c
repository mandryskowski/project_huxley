#include "label.h"
#include <stddef.h>
#include <string.h>
uint64_t getLabelAddress(char* name, Label* label)
{
	while (label->name != NULL)
	{
		if (!strcmp(name, label->name))
		{
			return label->address;
		}
		label++;
	}

	return UINT64_C(-1); // return max uint64_t
}
