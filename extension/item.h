#ifndef ARMV8_25_ITEM_H
#define ARMV8_25_ITEM_H

#include "entity.h"

typedef void (*passive)(Player *);
typedef void (*active)(Player *);

typedef enum ItemType
{
    BOOM_BOOTS, OVERCLOCKING_MODULE, CLONING_MODULE, ATTACK_MODULE, REBOUND_MODULE, HOLOGRAPHIC_MODULE, RESILIENCE_MODULE,
    ROCKET_BOOTS, JETPACK, STOPWATCH
} ItemType;

typedef struct Item
{
    Dialogue *dialogue;
    int textureID;
    passive item_passive;
    active item_active;
    int active_cooldown;
    int cooldown_left;
} Item;

Item *get_item(ItemType type);

Item *cpy_item(Item *item);

void free_item(Item *item);

#endif //ARMV8_25_ITEM_H
