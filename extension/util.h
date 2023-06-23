#ifndef ARMV8_25_UTIL_H
#define ARMV8_25_UTIL_H

#include "room.h"

void swap(void **this, void **other);

void shuffle(void *array, int num_elements, int element_size);

bool isOutOfBounds(Vec2i a, Room* room);

#endif //ARMV8_25_UTIL_H
