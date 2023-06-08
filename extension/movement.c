#include "GLFW/glfw3.h"
#include "state.h"
#include <math.h>
#include "math.h"
#include "entity.h"
#include "room.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

void playerMovement(GameState* state, float dt)
{
//    Player* player = state->player;
//    if (glfwGetKey(state->window, GLFW_KEY_W) == GLFW_PRESS)
//    {
//        player->entity.velocity += {0.0f, dt};
//    }
//    if (glfwGetKey(state->window, GLFW_KEY_S) == GLFW_PRESS)
//    {
//        player->entity.velocity += {0.0f, -dt};
//    }
//    if (glfwGetKey(state->window, GLFW_KEY_D) == GLFW_PRESS)
//    {
//        player->entity.velocity += {dt, 0.0f};
//    }
//    if (glfwGetKey(state->window, GLFW_KEY_A) == GLFW_PRESS)
//    {
//        player->entity.velocity += {-dt, 0.0f};
//    }
//
//    player->entity.velocity = player->entity.SPD * normalize(player->entity.velocity);


}

Vec2f detectCollisionRect(Rectangle a, Rectangle b, Vec2f velocity)
{
//    float overlapX = velocity.x > 0 ? b.topRight.x - a.bottomLeft.x : a.topRight.x - b.bottomLeft.x;
//    float overlapY = velocity.y > b.topRight.y ? b.topRight.y - a.bottomLeft.y : a.topRight.y - b.bottomLeft.y;
    float overlapX = a.topRight.x > b.topRight.x ? b.topRight.x - a.bottomLeft.x : a.topRight.x - b.bottomLeft.x;
    float overlapY = a.topRight.y > b.topRight.y ? b.topRight.y - a.bottomLeft.y : a.topRight.y - b.bottomLeft.y;
    printf("%f x overlap\n", overlapX);
    return (Vec2f){overlapX, overlapY};
}

void checkForCollision(Rectangle currHitbox, Rectangle otherHitbox, float *highestAfterCollision,  Vec2f *newVelocity, Vec2f velocity)
{
    Vec2f collisionResult = detectCollisionRect(currHitbox, otherHitbox, velocity);
    //printf("xddd\n");
    if (collisionResult.x > 0 && collisionResult.y > 0)
    {
        printf("xddd ____\n");
        Rectangle_print(currHitbox);
        printf("\n");
        Rectangle_print(otherHitbox);
        printf(" \n______\n");
        float xtime = velocity.x ? fabs(collisionResult.x / velocity.x) : 1e9;
        float ytime = velocity.y ? fabs(collisionResult.y / velocity.y) : 1e9;
        float timeAfterCollision = min(xtime, ytime);
        printf("%f %f something\n", xtime, ytime);
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

float moveUnitlPossible(Entity **entity, Entity **currEntityPtr, float dt, Rectangle *obstacles, Rectangle *obstaclesEnd)
{
    float highestAfterCollision = 0;
    Entity *currEntity = *currEntityPtr;
    Vec2f newVelocity = currEntity->velocity;
    Vec2f currEntityNewPos = Vec2f_add(currEntity->pos, Vec2f_scale(currEntity->velocity, dt));
    Rectangle currHitbox = rectangle_Vec2f(currEntity->hitbox, currEntityNewPos);
    Vec2f_print(currEntityNewPos);
    printf(" new pos to hit\n");

    for (Entity **otherPtr = entity; *otherPtr; otherPtr++)
    {
        if (otherPtr == currEntityPtr)
        {
            continue;
        }
        Entity *other = *otherPtr;

        Vec2f currEntityNewPos = Vec2f_add(currEntity->pos, Vec2f_scale(currEntity->velocity, dt));
        Rectangle currHitbox = rectangle_Vec2f(currEntity->hitbox, currEntityNewPos);
        Rectangle otherHitbox = rectangle_Vec2f(other->hitbox, other->pos);

        checkForCollision(currHitbox, otherHitbox, &highestAfterCollision, &newVelocity, currEntity->velocity);
    }

    for (Rectangle *obstacle = obstacles; obstacle < obstaclesEnd; obstacle++)
    {
        Rectangle otherHitbox = *obstacle;

        checkForCollision(currHitbox, otherHitbox, &highestAfterCollision, &newVelocity, currEntity->velocity);
    }

    //Vec2f_print(currEntity->velocity);
    //printf(" %f xddd\n", highestAfterCollision);
    printf("%p pointer\n", currEntity);
    Vec2f_print(currEntity->velocity);
    printf(" velocity\n");
    Vec2f_print(currEntity->pos);
    printf(" before move\n");
    currEntity->pos = Vec2f_add(currEntity->pos, Vec2f_scale(currEntity->velocity, dt - highestAfterCollision));
    Vec2f_print(currEntity->pos);
    printf(" after move\n");
    printf("scalar: %f %f\n", dt, highestAfterCollision);
    currEntity->velocity = newVelocity;
    return highestAfterCollision;
}

void move(GameState* state, Entity** entity, float dt)
{
    const int NUM_OF_STEPS = 36;
    for (Entity **currEntity = entity; *currEntity; currEntity++)
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
//        for (Rectangle *i = obstacles; i < obstaclesEnd; i++)
//        {
//            Rectangle_print(*i);
//            printf("\n");
//        }
//        exit(0);
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