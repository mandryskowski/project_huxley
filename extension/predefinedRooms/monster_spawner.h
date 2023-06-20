#ifndef MONSTER_SPAWNER_H
#define MONSTER_SPAWNER_H

#include "../room.h"
#include "../entity.h"
#include "../game_math.h"
#include "generator_attributes.h"

MonsterType **spawn_monsters(Room* room, Mode mode, RoomType type);

#endif //MONSTER_SPAWNER_H