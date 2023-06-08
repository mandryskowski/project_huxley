#include "GLFW/glfw3.h"
#include "state.h"
#include "math.h"
#include "entity.h"
#include "room.h"
#include <stdbool.h>
#include <stdlib.h>

void playerMovement(GameState* state, float dt)
{
    Player* player = state->player;
    if (glfwGetKey(state->window, GLFW_KEY_W) == GLFW_PRESS)
    {
        player->entity.velocity += {0.0f, dt};
    }
    if (glfwGetKey(state->window, GLFW_KEY_S) == GLFW_PRESS)
    {
        player->entity.velocity += {0.0f, -dt};
    }
    if (glfwGetKey(state->window, GLFW_KEY_D) == GLFW_PRESS)
    {
        player->entity.velocity += {dt, 0.0f};
    }
    if (glfwGetKey(state->window, GLFW_KEY_A) == GLFW_PRESS)
    {
        player->entity.velocity += {-dt, 0.0f};
    }

    player->entity.velocity = player->entity.SPD * normalize(player->entity.velocity);


}

Vec2f detectCollisionRect(Rectangle a, Rectangle b)
{
    float overlapX = a.topRight.x > b.topRight.x ? b.topRight.x - a.bottomLeft.x : a.topRight.x - b.bottomLeft.x;
    float overlapY = a.topRight.y > b.topRight.y ? b.topRight.y - a.bottomLeft.y : a.topRight.y - b.bottomLeft.y;
    return (Vec2f){overlapX, overlapY};
}

void checkForCollision(Rectangle currHitbox, Rectangle otherHitbox, float *highestAfterCollision,  Vec2f *newVelocity, Vec2f velocity)
{

    Vec2f collisionResult = detectCollisionRect(currHitbox, otherHitbox);
    if (collisionResult.x > 0 && collisionResult.y > 0)
    {
        float timeAfterCollision = min(abs(collisionResult.x / velocity.x),
                                       abs(collisionResult.y / velocity.y));
        if (timeAfterCollision > *highestAfterCollision)
        {
            *highestAfterCollision = timeAfterCollision;
            if (collisionResult.x / velocity.x <= collisionResult.y / velocity.y)
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

float moveUnitlPossible(Entity **entity, Entity **currEntityPtr, float dt, Rectangle *obstacles, Rectangle *obstaclesEnd)
{
    float highestAfterCollision = 0;
    Vec2f newVelocity;
    Entity *currEntity = *currEntityPtr;
    Vec2f currEntityNewPos = Vec2f_add(currEntity->pos, Vec2f_scalar(currEntity->velocity, dt));
    Rectangle currHitbox = rectangle_Vec2f(currEntity->hitbox, currEntityNewPos);

    for (Entity **otherPtr = entity; otherPtr; otherPtr++)
    {
        if (otherPtr == currEntityPtr)
        {
            continue;
        }
        Entity *other = *otherPtr;

        Vec2f currEntityNewPos = Vec2f_add(currEntity->pos, Vec2f_scalar(currEntity->velocity, dt));
        Rectangle currHitbox = rectangle_Vec2f(currEntity->hitbox, currEntityNewPos);
        Rectangle otherHitbox = rectangle_Vec2f(other->hitbox, other->pos);

        checkForCollision(currHitbox, otherHitbox, &highestAfterCollision, &newVelocity, currEntity->velocity);
    }
    for (Rectangle *obstacle = obstacles; obstacle < obstaclesEnd; obstacle++)
    {
        Rectangle otherHitbox = *obstacle;

        checkForCollision(currHitbox, otherHitbox, &highestAfterCollision, &newVelocity, currEntity->velocity);
    }

    currEntity->pos = Vec2f_add(currEntity->pos, Vec2f_scalar(currEntity->velocity, dt - highestAfterCollision));
    currEntity->velocity = newVelocity;
    return highestAfterCollision;
}

void move(GameState* state, Entity** entity, float dt)
{

    const int NUM_OF_STEPS = 3;
    for (Entity **currEntity = entity; currEntity; currEntity++)
    {
        Rectangle *obstacles = calloc(8, sizeof(Rectangle));
        Rectangle *obstaclesEnd = obstacles;
        for (int i = -1; i < 2; i++)
        {
            for (int j = -1; j < 2; j++)
            {
                Vec2i tile = {(int)(*currEntity)->pos.x + i, (int)(*currEntity)->pos.y + j};
                if (getTile(tile, state) != TILE_FLOOR)
                {
                    *obstaclesEnd++ = (Rectangle){{tile.x, tile.y}, {tile.x + 1, tile.y + 1}};
                }
            }
        }

        for (int i = 0; i < NUM_OF_STEPS; i++)
        {
            float timePerStep = dt / NUM_OF_STEPS;
            while (timePerStep && !Vec2f_zero((*currEntity)->velocity))
            {
                timePerStep = moveUnitlPossible(entity, currEntity, timePerStep, obstacles, obstaclesEnd);
            }
        }
        free(obstacles);
    }
}

Vec2f* findPath(GameState* state, Entity* entity, Vec2f desiredPosition)
{
    
}