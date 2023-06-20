#ifndef GAME_H
#define GAME_H

#include "room.h"
#include "state.h"
void initGame(GameState*);
void gameLoop(GameState*);
void disposeOfGame(GameState*);
void erase_dead(Room*);
void menuLoop(GameState* gState);
#endif // GAME_H