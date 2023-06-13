#ifndef STRUCTURE_BUILDER_H
#define STRUCTURE_BUILDER_H

#include "../entity.h"
#include "../game_math.h"
#include "generator_attributes.h"


void patternBuilder(Room *room, Pattern pattern, Vec2i topLeft, Vec2i bottomRight, TileType type);
void presetStructures(Room *room, Structure structure, Vec2i topLeft, Vec2i bottomRight, TileType type, int angle);

#endif // STRUCTURE_BUILDER_H