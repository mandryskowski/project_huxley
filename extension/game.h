#ifndef GAME_H
#define GAME_H

#include "room.h"
#include "state.h"
void initGame(GameState*);
void gameLoop(GameState*);
void erase_dead(Room*);
#endif // GAME_H