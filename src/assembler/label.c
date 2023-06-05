#include "label.h"
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
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

	return MAX_UINT64T; // if not found
}
