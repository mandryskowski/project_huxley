#include "emulateUtility.h"


bool getBit(int pos, int64_t instruction)
{
    return instruction & (1 << pos);
}

// Returns bits at the interval <start, end> (interval is closed) and does unsigned extend.
uint64_t getBits(int start, int end, int64_t instruction)
{
    if(end != 63)
        return (instruction &  ((1ll << (end + 1)) - (1ll << start))) >> start;
    return instruction >> start;
}

int64_t getBitsSignExt(int start, int end, int64_t instruction)
{
    const int64_t mask = getBit(end, instruction) ? INT64_MAX : 0ll;
    return (mask << (end - start)) | getBits(start, end, instruction);
}

// Returns type of the instruction
instructionType getInstructionType(int instruction)
{
    int bitmasks[] = {0b1000, 0b0101, 0b0100, 0b1010};
    int dontCares[] = {0b1110, 0b0111, 0b0101, 0b1110};
    int opcode = getBits(25, 28, instruction);
    instructionType type = UNDEFINED;

    for (instructionType i = FIRST; i <= LAST; i++)
    {
        if ((dontCares[i] & opcode) == bitmasks[i])
        {
            type = i;
        }
    }
    return type;
}