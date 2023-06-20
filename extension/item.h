#ifndef ARMV8_25_ITEM_H
#define ARMV8_25_ITEM_H

#include "entity.h"

typedef void (*passive)(Player *);
typedef void (*active)(Player *);

typedef struct Item
{
    Dialogue *dialogue;
    int textureID;
//    char *name;
//    char *description;
    passive item_passive;
    active item_active;
    int active_cooldown;
    int cooldown_left;
} Item;

Item *construct_stopwatch();

#endif //ARMV8_25_ITEM_H
