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
	    operand = (int32_t)operand; // Take bottom 32 bits and sign extend.
    }
    operand = operand >> amount;
    return is64bit ? operand : (int32_t)operand;
}

int64_t RotateRight(int64_t operand, int amount, bool is64bit)
{
	  if (!is64bit)
    {
      operand = (uint32_t)operand; // Take bottom 32 bits and unsigned extend.
    }
	  operand = LogicalSR(operand, amount, is64bit) | LogicalSL(operand, (is64bit ? 64 : 32) - amount, is64bit);
    return is64bit ? operand : (int32_t)operand;	
}

int64_t ExecuteShift(int shiftType, int64_t operand, int amount, bool is64bit)
{
    switch(shiftType) {
      case 0b00:
        return LogicalSL(operand, amount, is64bit);
        break;
      case 0b01:
        return LogicalSR(operand, amount, is64bit);
        break;
      case 0b10:
        return ArithmeticSR(operand, amount, is64bit);
        break;
      case 0b11:
        return RotateRight(operand, amount, is64bit);
        break;
      default:
        perror("Invalid shift type");
        return -1;
    }
}