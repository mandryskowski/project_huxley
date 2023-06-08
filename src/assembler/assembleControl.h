#ifndef ARMV8_25_ASSEMBLECONTROL_H
#define ARMV8_25_ASSEMBLECONTROL_H

#include <stdint.h>

typedef enum {DP_ASS, BRANCH_ASS, SDT_ASS, SPECIAL_ASS, FIRST_ASS = DP_ASS,
              LAST_ASS = SPECIAL_ASS, UNDEFINED_ASS} assembleType;

int32_t assembleInstruction(char** tokens, uint64_t PC);

#endif //ARMV8_25_ASSEMBLECONTROL_H
