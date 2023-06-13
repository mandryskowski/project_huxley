#include <stdlib.h>
#include <stdio.h>
#include "../room.h"
#include "../entity.h"
#include "../game_math.h"
#include "generator_attributes.h"
#include "monster_spawner.h"    

#define AVG(a,b) ((a+b)/2.0)

double position_chance(Vec2i current_i, Room* room, Mode mode)
{
    Vec2d current = Vec2i_to_Vec2d(current_i);

    Vec2d middle;
    middle.x = ((double)(room->width)) / 2.0;
    middle.y = ((double)(room->height)) / 2.0;

    //Distance from the doors
    double leftDistance = Vec2d_metric_distance(current,(Vec2d) {0, middle.y});
    double rightDistance = Vec2d_metric_distance(current, (Vec2d) {2.0 * middle.x , middle.y});

    //Closest door to the mob
    double distance = min(leftDistance, rightDistance);

    //20% higher chance of mobs spawning every time the mode becomes more intense
    double modifier = ((double) mode) * (0.2) + 1;

    return modifier * (distance / middle.x);

}

void put_monsters(MonsterType** monsters, Room* room, Mode mode)
{
    for (int i = 0; i < room->height; i++)
    {
        for (int j = 0; j < room->width; j++)
        {
            if (room->tiles[i][j].type != TILE_FLOOR)
            {
                monsters[i][j] = NOT_MONSTER;
                continue;
            }

            double prob = AVG(((double) (rand() % 1000)) ,
                              1000 * position_chance((Vec2i) {j, i}, room, mode));
            double threshold = ((double) mode) * 50;

            if (prob < 100 + threshold)
            {
                monsters[i][j] = ZOMBIE;
            }
            if (prob < 80 + threshold)
            {
                monsters[i][j] = SHOOTER;
            }
            if (prob < 20 + threshold)
            {
                monsters[i][j] = FLYING_SHOOTER;
            }
            else
            {
             monsters[i][j] = NOT_MONSTER;
            }
        }
    }
}

MonsterType **spawn_monsters(Room* room, Mode mode)
{
    //Allocating monsters mem.
    MonsterType** monsters = calloc(room->height, sizeof(MonsterType*));
    for(int i = 0; i < room->height; i++)
    {
        monsters[i] = calloc(room->width, sizeof(MonsterType));
        if(monsters[i] == NULL) {exit(EXIT_FAILURE);}
    }
    if(monsters == NULL) {exit(EXIT_FAILURE);}

    put_monsters(monsters, room, mode);

    return monsters;
}