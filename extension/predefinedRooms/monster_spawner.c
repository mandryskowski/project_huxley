#include <stdlib.h>
#include <stdio.h>
#include <time.h>
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
    middle.x = ((double)(room->size.x)) / 2.0;
    middle.y = ((double)(room->size.y)) / 2.0;

    //Closest door to the mob
    double distance = Vec2d_metric_distance(middle, current);

    //20% higher chance of mobs spawning every time the mode becomes more intense
    double modifier = ((double) mode) * (0.2) + 1;

    return 1 - 1.0/distance ;

}

void put_monsters(MonsterType** monsters, Room* room, Mode mode)
{
    srand(time(NULL));
    for (int i = 0; i < room->size.y; i++)
    {
        for (int j = 0; j < room->size.x; j++)
        {
            if (room->tiles[i][j].type != TILE_FLOOR)
            {
                monsters[i][j] = NOT_MONSTER;
                continue;
            }

            double prob = AVG((double) (rand() % 1000) , 1000 * position_chance((Vec2i) {j, i}, room, mode));
            double threshold = ((double) mode) * 50;

            monsters[i][j] = NOT_MONSTER;
            //printf("%f\n", prob);

            if (prob < 250 + threshold)
            {
                monsters[i][j] = BOMBER;
            }
            if (prob < 200 + threshold)
            {
                monsters[i][j] = ZOMBIE;
            }
            if (prob < 150 + threshold)
            {
                monsters[i][j] = SHOOTER;
            }
            if (prob < 50 + threshold)
            {
                monsters[i][j] = FLYING_SHOOTER;
            }
        
        }
    }
}

MonsterType **spawn_monsters(Room* room, Mode mode, RoomType type)
{

    //Allocating monsters mem.
    MonsterType** monsters = calloc(room->size.y, sizeof(MonsterType*));
    for(int i = 0; i < room->size.y; i++)
    {
        monsters[i] = calloc(room->size.x, sizeof(MonsterType));
        if(monsters[i] == NULL) {exit(EXIT_FAILURE);}
    }
    if(monsters == NULL) {exit(EXIT_FAILURE);}

    if (type == ITEM_ROOM)
    {
        for (int i = 0; i < room->size.y; i++)
        {
            for (int j = 0; j < room->size.x; j++)
            {
                monsters[i][j] = NOT_MONSTER;
            }
        }
        return monsters;
    }

    put_monsters(monsters, room, mode);

    return monsters;
}