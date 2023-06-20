#include "item.h"
#include <stdlib.h>

void flying(Player *player)
{
    player->entity->canFly = true;
}

void movement_speed_increase(Player *player)
{
    player->entity->SPD += 1;
}

void attack_cooldown_decrease(Player *player)
{
    player->entity->attack_cooldown /= 2;
}

void attack_damage_increase(Player *player)
{
    player->entity->ATK += 10;
}

void extra_bullet(Player *player)
{
    player->entity->attack_modifier += 1;
}

void random_mine_spawn(Player *player)
{
    player->throws_mines = true;
}

void time_travel(Player *player)
{
    player->entity->pos = pop(player->prev_positions);
}

Item construct_jetpack()
{
    return (Item){.textureID = 0, .name = "jetpack", .item_passive = flying, .description = "Grants flying to konstantinos"};
}

Item construct_boots()
{
    return (Item){.textureID = 0, .name = "boots", .item_passive = movement_speed_increase, .description = "Grants more movement speed"};
}

Item construct_nano_cooling_system()
{
    return (Item){.textureID = 0, .name = "Nano Cooling System", .item_passive = movement_speed_increase, .description = "Reduces cool-down of attacks"};
}

Item construct_bigger_breadboards()
{
    return (Item){.textureID = 0, .name = "Bigger breadboards", .item_passive = movement_speed_increase, .description = "Increases attack damage"};
}

Item construct_overclocking_module()
{
    return (Item){.textureID = 0, .name = "Overclocking module", .item_passive = movement_speed_increase, .description = "konstantinos throws one more breadboard"};
}

Item construct_unstable_mines()
{
    return (Item){.textureID = 0, .name = "Unstable mines", .item_passive = movement_speed_increase, .description = "Randomly throws mines that detonate after 5s"};
}

Item *construct_stopwatch()
{
    Item *stopwatch = malloc(sizeof(Item));
    *stopwatch = (Item){.textureID = 0, .item_active = time_travel, .active_cooldown = 300};
    return stopwatch;
}