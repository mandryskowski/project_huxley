#ifndef ARMV8_25_ASSEMBLECONTROL_H
#define ARMV8_25_ASSEMBLECONTROL_H


// Interval is [start, end)
char *substr(char *string, int start, int end);

// splits instruction into words
char **split(char *instruction);

void setBits(int *instruction, int mask, int start);

#endif //ARMV8_25_ASSEMBLECONTROL_H
