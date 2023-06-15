#ifndef LEVEL_H
#define LEVEL_H

#include "game_math.h"

typedef struct Room Room;
typedef struct GameState GameState;

typedef struct Level
{
    Room ***map;
    Room *currentRoom, *prevRoom;
    Vec2i currRoomCoords, prevRoomCoords;
} Level;

void jump_to_next_room(GameState *state);
Level *construct_level(Player *player, int room_number);

#endif //LEVEL_H
