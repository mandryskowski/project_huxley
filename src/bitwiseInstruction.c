#include "bitwiseInstruction.h"
#include "control.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MASK (1LL << 32) - 1

typedef struct CarryPair {
	int64_t shift;
	int64_t trunc;
}CarryPair;

CarryPair LogicalSL(int64_t* operand, int amount, int truncate)
{
  int length = (truncate) ? 32 : 64;
  CarryPair carry;
  carry.shift = getBits(length - amount, length - 1, *operand);
  carry.trunc = (truncate) ? getBits(32, 63, *operand) : 0;
  *operand <<= amount;
  if(truncate)
  {
    *operand &= MASK; 
  }
  return carry;
}


CarryPair LogicalSR(int64_t* operand, int amount, int truncate)
  {
    CarryPair carry;
    carry.shift = getBits(0, amount - 1, *operand);
    carry.trunc = (truncate) ? getBits(32, 63, *operand) : 0;
    *operand >>= amount;
    if(truncate)
    {
      *operand &= MASK;
    }
    return carry;
  }

CarryPair ArithmeticSR(int64_t *operand, int amount, int truncate)
 {
   int length = (truncate) ? 32 : 64;
   int64_t firstbit = getBits(length -1, length -1 , *operand);
   CarryPair carry;
   carry.shift = getBits(0, amount - 1, *operand);
   carry.trunc = (truncate) ? getBits(32, 63, *operand) : 0;
   *operand >>= amount;
   if(truncate)
  {
     *operand &= MASK;
  }
  if(firstbit)
  {
  int64_t arithmask = ((1ll << (length)) - (1ll << (length - amount)));
  *operand |= arithmask;
  }
  return carry;
}

CarryPair RotateRight(int64_t *operand, int amount, int truncate)
{
  int length = (truncate) ? 32 : 64;
  CarryPair carry;
  carry.shift = getBits(0, amount - 1, *operand);
  carry.trunc = (truncate) ? getBits(32, 63, *operand) : 0;
  *operand >>= amount;
  if(truncate)
  {
    *operand &= MASK;
  }
  *operand |= carry.shift << (length - amount);
  return carry;
}

CarryPair executeShift(int shiftType, int *operand, int amount, int truncate)
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
