#ifndef ARMV8_25_ITEM_H
#define ARMV8_25_ITEM_H

#include "entity.h"

typedef void (*passive)(Player *);

typedef struct Item
{
    int textureID;
    char *name;
    char *description;
    passive item_passive;
} Item;

#endif //ARMV8_25_ITEM_H
