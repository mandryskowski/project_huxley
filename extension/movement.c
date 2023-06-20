#include "glfw/glfw3.h"
#include "state.h"
#include <math.h>
#include "game_math.h"
#include "entity.h"
#include "room.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "level.h"
#include "util.h"

void playerMovement(GameState* state, double dt)
{

}

static bool is_clear;

void deal_collison_damage(Entity *entity, Entity *other)
{
    if (isMine(other))
    {
        other->cooldown_left = 0;
    }
    handle_attack(entity, other, ATTACK_CONTACT);
    handle_attack(other, entity, ATTACK_CONTACT);
}

Vec2d detectCollisionRect(Rectangle a, Rectangle b)
{
    double overlapX = a.topRight.x > b.topRight.x ? b.topRight.x - a.bottomLeft.x : a.topRight.x - b.bottomLeft.x;
    double overlapY = a.topRight.y > b.topRight.y ? b.topRight.y - a.bottomLeft.y : a.topRight.y - b.bottomLeft.y;

    return (Vec2d){overlapX + EPSILON, overlapY + EPSILON};
}

bool checkForCollision(Rectangle currHitbox, Rectangle otherHitbox, double *highestAfterCollision,  Vec2d *newVelocity, Vec2d velocity, bool is_projectile)
{
    Vec2d collisionResult = detectCollisionRect(currHitbox, otherHitbox);
    
    if (collisionResult.x > 0 && collisionResult.y > 0)
    {
        if (highestAfterCollision == NULL)
        {
            return true;
        }
        double xtime = velocity.x ? fabs(((double)collisionResult.x + EPSILON / 2) / velocity.x) : 1e9;
        double ytime = velocity.y ? fabs(((double)collisionResult.y + EPSILON / 2) / velocity.y) : 1e9;
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
        return true;
    }
    return false;
}

double moveUnitlPossible(Entity **entity, Entity **currEntityPtr, double dt, Rectangle *obstacles, GameState *state, bool isclear)
{
    double highestAfterCollision = 0;
    Entity *currEntity = *currEntityPtr;
    Vec2d newVelocity = currEntity->velocity;
    Vec2d currEntityNewPos = Vec2d_add(currEntity->pos, Vec2d_scale(currEntity->velocity, dt));
    Rectangle currHitbox = rectangle_Vec2d(currEntity->hitbox, currEntityNewPos);

    bool canEnterDialogue = false;

    for (Entity **otherPtr = entity; *otherPtr; otherPtr++)
    {
        //printf("%d %d %d %d %d\n", (otherPtr == currEntityPtr), (*otherPtr)->faction, isDead(*otherPtr), isProjectile(*otherPtr), isMine(*otherPtr));
        if (otherPtr == currEntityPtr || isDead(*otherPtr) ||
        ((isProjectile(*otherPtr) || isProjectile(*currEntityPtr)) && (*otherPtr)->faction == (*currEntityPtr)->faction)
        || (isProjectile(*otherPtr) && isProjectile(*currEntityPtr)) || (isMine(*otherPtr) && (*otherPtr)->faction == (*currEntityPtr)->faction) )
        {
            continue;
        }
        Entity *other = *otherPtr;

        if(isNPC(other) && Vec2d_metric_distance(currEntity->pos, other->pos) < 1.4)
        {
            canEnterDialogue = true;
            continue;
        }

        Rectangle otherHitbox = rectangle_Vec2d(other->hitbox, other->pos);

        if (isPickable(other))
        {
            if (currEntityPtr == entity){
                if (isKatsu(other) && currEntity->HP == currEntity->maxHP)
                {
                    continue;
                }
            }
            else
            {
                continue;
            }
        }

        if (checkForCollision(rectangle_Vec2d(currEntity->hitbox, currEntity->pos), otherHitbox, NULL, NULL, (Vec2d){0, 0}, false))
        {
            continue;
        }

        double highestAfterCollision_prev = highestAfterCollision;
        Vec2d prev_newVelocity = newVelocity;
        bool collides = checkForCollision(currHitbox, otherHitbox, &highestAfterCollision,
                                          &newVelocity, currEntity->velocity, isProjectile(currEntity));
        if (collides)
        {
            deal_collison_damage(currEntity, other);
            if (isProjectile(currEntity) || isPickable(other))
            {
                highestAfterCollision = highestAfterCollision_prev;
                newVelocity = prev_newVelocity;
            }
            if (isDead(*currEntityPtr))
            {
                return 0; // Entity is dead
            }
        }
        if (isProjectile(currEntity) && currEntity->faction == ENEMY)
        {
            break;
        }
    }

    state->player->canEnterDialogue = canEnterDialogue & (!state->player->isInDialogue);

    for (Rectangle *obstacle = obstacles; obstacle->topRight.x; obstacle++)
    {
        Rectangle otherHitbox = *obstacle;
        bool collides = checkForCollision(currHitbox, otherHitbox, &highestAfterCollision,
                                          &newVelocity, currEntity->velocity, false);

        if (collides)
        {
            if (isProjectile(currEntity))
            {
                if (currEntity->projectileStats.bounces)
                {
                    currEntity->projectileStats.bounces--;
                    currEntity->pos = Vec2d_add(currEntity->pos, Vec2d_scale(currEntity->velocity, dt - highestAfterCollision));
                    Vec2d wall_vector = fabs(newVelocity.x) < EPSILON ? (Vec2d){0, 1} : (Vec2d){-1, 0};
                    if (fabs(newVelocity.x) < EPSILON && currEntity->velocity.x < 0)
                    {
                        wall_vector = Vec2d_scale(wall_vector, -1);
                    }
                    else if (fabs(newVelocity.y) < EPSILON && currEntity->velocity.y < 0)
                    {
                        wall_vector = Vec2d_scale(wall_vector, -1);
                    }
                    double angle = angle_between_Vec2d(currEntity->velocity, wall_vector);
                    //Vec2d_print(wall_vector);
                    //printf(" angle %f\n", angle);
                    currEntity->velocity = Vec2d_rotate(currEntity->velocity, 2 * angle);
                    return highestAfterCollision;
                }
                else
                {
                    killEntity(currEntity);
                    return  0; // Entity is dead
                }
            }
        }
    }

    currEntity->pos = Vec2d_add(currEntity->pos, Vec2d_scale(currEntity->velocity, dt - highestAfterCollision));
    currEntity->velocity = newVelocity;
    return highestAfterCollision;
}

void add_wall(Vec2i cBounds, int valBound, bool isX, Rectangle **obstaclesEnd, GameState *state, Entity *currEntity)
{
    for (int i = cBounds.x; i <= cBounds.y; i++)
    {
        Vec2i tile = isX ? (Vec2i){valBound, i} : (Vec2i){i, valBound};

        if (isOutOfBounds(tile, state->currentLevel->currentRoom))
        {
            continue;
        }

        if (getTile(tile, state) == TILE_DOOR && currEntity == state->player->entity && is_clear)
        {
            //printf("xdd\n");
            continue;
        }
        if (isProjectile(currEntity))
        {
            if (getTile(tile, state) != TILE_FLOOR && getTile(tile, state) != TILE_HOLE)
            {
                *(*obstaclesEnd)++ = (Rectangle){{tile.x, tile.y}, {tile.x + 1, tile.y + 1}};
            }
        }
        else if ((!currEntity->canFly || getTile(tile, state) == TILE_WALL || getTile(tile, state) == TILE_DOOR) && getTile(tile, state) != TILE_FLOOR)
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
        add_wall(y_bounds, currHitbox.topRight.x + 1, true, &obstaclesEnd, state, currEntity);
    }
    else
    {
        add_wall(y_bounds, currHitbox.bottomLeft.x - 1, true, &obstaclesEnd, state, currEntity);
    }

    if (currEntity->velocity.y > 0)
    {
        add_wall(x_bounds, currHitbox.topRight.y + 1, false, &obstaclesEnd, state, currEntity);
    }
    else
    {
        add_wall(x_bounds, currHitbox.bottomLeft.y - 1, false, &obstaclesEnd, state, currEntity);
    }
}

void move(GameState* state, Entity** entity, double dt)
{
    const int NUM_OF_STEPS = 1;
    is_clear = isClear(state->currentLevel->currentRoom);

    for (Entity **currEntity = entity; *currEntity; currEntity++)
    {
        Rectangle *obstacles = calloc(8, sizeof(Rectangle));
        add_potential_obstacles(obstacles, *currEntity, state);

        for (int i = 0; i < NUM_OF_STEPS; i++)
        {
            double timePerStep = dt / NUM_OF_STEPS;
            while (timePerStep && !Vec2d_zero((*currEntity)->velocity) && !isDead(*currEntity))
            {
                timePerStep = moveUnitlPossible(entity, currEntity, timePerStep, obstacles, state, is_clear);
            }
        }
        free(obstacles);
        if (currEntity!= entity && isOutOfBounds(Vec2d_to_Vec2i((*currEntity)->pos), state->currentLevel->currentRoom))
        {
            killEntity(*currEntity);
        }
    }

    push(state->player->prev_positions, state->player->entity->pos);
    if (isOutOfBounds(Vec2d_to_Vec2i(state->player->entity->pos), state->currentLevel->currentRoom))
    {
        state->renderNewRoom = true;
        clearQueue(state->player->prev_positions);
    }
}

