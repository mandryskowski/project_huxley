//
// Created by Jakub Łapiński on 26/05/2023.
//

#ifndef ARMV8_25_CONTROL_H
#define ARMV8_25_CONTROL_H

typedef enum  {IMMEDIATE, REGISTER, LOADSTORE, BRANCH, FIRST = IMMEDIATE, LAST = BRANCH, UNDEFINED} instructionType;

int getBits(int start, int end, int instruction);

instructionType getInstructionType(int instruction);

#endif //ARMV8_25_CONTROL_H
