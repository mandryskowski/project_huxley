#include "bitwiseInstruction.h"
#include "emulateUtility.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


int64_t LogicalSL(int64_t operand, int amount, bool is64bit)
{
    operand = (uint64_t) operand << amount; // Logical shift is performed on unsigned values so we cast to uint64_t
    return is64bit ? operand : getBits(0, 31, operand);
}


int64_t LogicalSR(int64_t operand, int amount, bool is64bit)
{
    operand = (uint64_t) operand >> amount; // Again, we cast to uint64_t to perform a logical shift
    return is64bit ? operand : getBits(0, 31, operand);
}

int64_t ArithmeticSR(int64_t operand, int amount, bool is64bit)
 {
    if (!is64bit)
    {
      operand = getBitsSignExt(0, 31, operand); // For 32-bit arithmetic shift we must sign extend truncated bits
    }

    operand = operand >> amount; // Arithmetic shift right is performed on signed values
    return is64bit ? operand : getBits(0, 31, operand);
}

int64_t RotateRight(int64_t operand, int amount, bool is64bit)
{
    if (!is64bit)
    {
      operand = getBits(0, 31, operand); // For 32-bit rotation we unsigned extend truncated bits.
    }
	  operand = LogicalSR(operand, amount, is64bit) | LogicalSL(operand, (is64bit ? 64 : 32) - amount, is64bit);
    return is64bit ? operand : getBits(0, 31, operand);	
}

int64_t ExecuteShift(int shiftType, int64_t operand, int amount, bool is64bit)
{
    switch(shiftType)
    {
      case 0x0: // 0b00
        return LogicalSL(operand, amount, is64bit);

      case 0x1: // 0b01
        return LogicalSR(operand, amount, is64bit);

      case 0x2: // 0b10
        return ArithmeticSR(operand, amount, is64bit);

      case 0x3: // 0b11
        return RotateRight(operand, amount, is64bit);

      default:
        perror("Invalid shift type");
        return -1;
    }
}
