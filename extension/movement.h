#ifndef MOVEMENT_H
#define MOVEMENT_H

typedef struct GameState GameState;
typedef struct Entity Entity;

void move(GameState* state, Entity** entity, double dt);

#endif // MOVEMENT_H