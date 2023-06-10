#include "glfw/glfw3.h"
#include "state.h"
#include <math.h>
#include "math.h"
#include "entity.h"
#include "room.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

void playerMovement(GameState* state, double dt)
{

}
Vec2d detectCollisionRect(Rectangle a, Rectangle b, Vec2d velocity)
{
    double overlapX = a.topRight.x > b.topRight.x ? b.topRight.x - a.bottomLeft.x : a.topRight.x - b.bottomLeft.x;
    double overlapY = a.topRight.y > b.topRight.y ? b.topRight.y - a.bottomLeft.y : a.topRight.y - b.bottomLeft.y;

    return (Vec2d){overlapX + EPSILON, overlapY + EPSILON};
}

void checkForCollision(Rectangle currHitbox, Rectangle otherHitbox, double *highestAfterCollision,  Vec2d *newVelocity, Vec2d velocity)
{
    Vec2d collisionResult = detectCollisionRect(currHitbox, otherHitbox, velocity);
    
    if (collisionResult.x > 0 && collisionResult.y > 0)
    {
        double xtime = velocity.x ? fabs(((double)collisionResult.x + EPSILON) / velocity.x) : 1e9;
        double ytime = velocity.y ? fabs(((double)collisionResult.y + EPSILON) / velocity.y) : 1e9;
        double timeAfterCollision = min(xtime, ytime);

        if (timeAfterCollision > *highestAfterCollision)
        {
            *highestAfterCollision = timeAfterCollision;
            if (xtime <= ytime)
            {
                newVelocity->y = velocity.y;
                newVelocity->x = 0;
            }
            else
            {
                newVelocity->x = velocity.x;
                newVelocity->y = 0;
            }
        }
    }
}

double moveUnitlPossible(Entity **entity, Entity **currEntityPtr, double dt, Rectangle *obstacles)
{
    double highestAfterCollision = 0;
    Entity *currEntity = *currEntityPtr;
    Vec2d newVelocity = currEntity->velocity;
    Vec2d currEntityNewPos = Vec2d_add(currEntity->pos, Vec2d_scale(currEntity->velocity, dt));
    Rectangle currHitbox = rectangle_Vec2d(currEntity->hitbox, currEntityNewPos);

    for (Entity **otherPtr = entity; *otherPtr; otherPtr++)
    {
        if (otherPtr == currEntityPtr)
        {
            continue;
        }
        Entity *other = *otherPtr;

        Vec2d currEntityNewPos = Vec2d_add(currEntity->pos, Vec2d_scale(currEntity->velocity, dt));
        Rectangle currHitbox = rectangle_Vec2d(currEntity->hitbox, currEntityNewPos);
        Rectangle otherHitbox = rectangle_Vec2d(other->hitbox, other->pos);

        checkForCollision(currHitbox, otherHitbox, &highestAfterCollision, &newVelocity, currEntity->velocity);
    }

    for (Rectangle *obstacle = obstacles; obstacle->topRight.x; obstacle++)
    {
        Rectangle otherHitbox = *obstacle;
        checkForCollision(currHitbox, otherHitbox, &highestAfterCollision, &newVelocity, currEntity->velocity);
    }

    currEntity->pos = Vec2d_add(currEntity->pos, Vec2d_scale(currEntity->velocity, dt - highestAfterCollision));
    currEntity->velocity = newVelocity;
    return highestAfterCollision;
}

void add_wall(Vec2i cBounds, int valBound, bool isX, Rectangle **obstaclesEnd, GameState *state)
{
    for (int i = cBounds.x; i <= cBounds.y; i++)
    {
        Vec2i tile = isX ? (Vec2i){valBound, i} : (Vec2i){i, valBound};
        
        if (getTile(tile, state) != TILE_FLOOR)
        {
            *(*obstaclesEnd)++ = (Rectangle){{tile.x, tile.y}, {tile.x + 1, tile.y + 1}};
        }
    }
}

void add_potential_obstacles(Rectangle *obstaclesEnd, Entity *currEntity, GameState *state)
{
    Rectangle currHitbox = rectangle_Vec2d(currEntity->hitbox, currEntity->pos);
    Vec2i x_bounds = currEntity->velocity.x >= 0 ? (Vec2i){currHitbox.bottomLeft.x,  currHitbox.topRight.x + 1}
        : (Vec2i){currHitbox.bottomLeft.x - 1,  currHitbox.topRight.x};
    Vec2i y_bounds = currEntity->velocity.y >= 0 ? (Vec2i){currHitbox.bottomLeft.y,  currHitbox.topRight.y + 1}
        : (Vec2i){currHitbox.bottomLeft.y - 1,  currHitbox.topRight.y};

    if (currEntity->velocity.x > 0)
    {
        add_wall(y_bounds, currHitbox.topRight.x + 1, true, &obstaclesEnd, state);
    }
    else
    {
        add_wall(y_bounds, currHitbox.bottomLeft.x - 1, true, &obstaclesEnd, state);
    }

    if (currEntity->velocity.y > 0)
    {
        add_wall(x_bounds, currHitbox.topRight.y + 1, false, &obstaclesEnd, state);
    }
    else
    {
        add_wall(x_bounds, currHitbox.bottomLeft.y - 1, false, &obstaclesEnd, state);
    }
}

void move(GameState* state, Entity** entity, double dt)
{
    const int NUM_OF_STEPS = 6;
    for (Entity **currEntity = entity; *currEntity; currEntity++)
    {
        Rectangle *obstacles = calloc(50, sizeof(Rectangle));
        add_potential_obstacles(obstacles, *currEntity, state);
        
        for (int i = 0; i < NUM_OF_STEPS; i++)
        {
            double timePerStep = dt / NUM_OF_STEPS;
            while (timePerStep && !Vec2d_zero((*currEntity)->velocity))
            {
                timePerStep = moveUnitlPossible(entity, currEntity, timePerStep, obstacles);
            }
        }
        free(obstacles);
    }
}