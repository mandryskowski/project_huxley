#ifndef MOVEMENT_H
#define MOVEMENT_H

typedef struct GameState GameState;
typedef struct Entity Entity;

void move(GameState* state, Entity** entity, double dt);
Vec2d detectCollisionRect(Rectangle a, Rectangle b);

#endif // MOVEMENT_H