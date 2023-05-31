#include "bitwiseInstruction.h"
#include "control.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
  
#define MASK (1LL << 32) - 1

CarryPair LogicalSL(int64_t* operand, int amount, int truncate)
{
    *operand <<= amount;
	if (!truncate)
		*operand = (int32_t)*operand;
}


CarryPair LogicalSR(int64_t* operand, int amount, int truncate)
  {
 	*operand = (uint64_t)*operand >> amount;
	if (!truncate)
		*operand = (int32_t)*operand;
  }

CarryPair ArithmeticSR(int64_t *operand, int amount, int truncate)
 {
    if(!truncate)
	*operand = (int32_t)*operand;
    *operand >>= amount;
    if (!truncate)
        *operand = (uint32_t)*operand;
}

CarryPair RotateRight(int64_t *operand, int amount, int truncate)
{
	if (!truncate)
	*operand = (uint32_t)*operand;
	*operand = ((uint64_t)*operand >> amount) | (*operand << ((truncate ? 64 : 32) - amount));
	if(!truncate)
	*operand = (uint32_t)*operand;
	
}

CarryPair ExecuteShift(int shiftType, int64_t *operand, int amount, int truncate)
{
    switch(shiftType) {
      case 0b00:
        return LogicalSL(operand, amount, truncate);
        break;
      case 0b01:
        return LogicalSR(operand, amount, truncate);
        break;
      case 0b10:
        return ArithmeticSR(operand, amount, truncate);
        break;
      case 0b11:
        return RotateRight(operand, amount, truncate);
        break;
      default:
        perror("Invalid shift type");
	exit(EXIT_FAILURE);
    }
}

/*
int main() {
  int64_t operand;
  int amount, truncation, type;
  printf("input operand, amount, truncation (0/1), type of shift (0/1/2/3)\n");
  scanf("%ld %d %d %d", &operand, &amount, &truncation, &type);
  CarryPair test;
  switch(type) 
  {
    case 0:
  	test =  LogicalSL(&operand, amount, truncation);
  	break;
    case 1:
	test = LogicalSR(&operand, amount, truncation);
	break;
    case 2:
	test = ArithmeticSR(&operand, amount, truncation);
	break;
    case 3:
	test = RotateRight(&operand, amount, truncation);
	break;
    default:
	printf("%ld\n", test.shift);
  }
printf("New value: %ld\nShift carry: %ld\n Trunc carry: %ld\n", operand, test.shift, test.trunc);
return 0;
}
*/
