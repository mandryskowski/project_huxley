#include "bitwiseInstruction.h"
#include "control.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MASK (1LL << 32) - 1




int64_t LogicalSL(int64_t operand, int amount, bool is64bit)
{
    operand = (uint64_t) operand << amount; // Logical shift is performed on unsigned values so we cast to uint64_t.
    return is64bit ? operand : (int32_t)operand;
}


int64_t LogicalSR(int64_t operand, int amount, bool is64bit)
{
    operand = (uint64_t) operand >> amount; // Again, we cast to uint64_t to perform a logical shift.
    return is64bit ? operand : (int32_t)operand;
}

int64_t ArithmeticSR(int64_t operand, int amount, bool is64bit)
 {
   if (!is64bit)
   {
	   operand = (int32_t)operand;
   }
    *operand = *operand >> amount;
    return is64bit ? operand : (int32_t)operand;
    
}

int64_t RotateRight(int64_t operand, int amount, int truncate)
{
	if (!truncate)
    {
        *operand = (uint32_t)*operand;
    }
	*operand = ((uint64_t)*operand >> amount) | (*operand << ((is64bit ? 64 : 32) - amount));
    return is64bit ? operand : (int32_t)operand;	
}

void ExecuteShift(int shiftType, int64_t *operand, int amount, int truncate)
{
    switch(shiftType) {
      case 0b00:
        LogicalSL(operand, amount, truncate);
        break;
      case 0b01:
        LogicalSR(operand, amount, truncate);
        break;
      case 0b10:
        ArithmeticSR(operand, amount, truncate);
        break;
      case 0b11:
        RotateRight(operand, amount, truncate);
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
