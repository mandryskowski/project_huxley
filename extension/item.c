#include "item.h"
#include <stdlib.h>

void flight(Player *player)
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

void max_health_increase(Player *player)
{
    player->entity->maxHP += 20;
}

void bouncy_projectiles(Player *player)
{
    player->entity->projectileStats->bounces = 2;
}

void piercing(Player *player)
{
    player->entity->projectileStats->pierces = 100;
}

void bomb_trail(Player *player)
{
    //player->entity->leavesBombs = true;
}

Item construct_boom_boots()
{
    return (Item){.textureID = 0, .name = "Boom Boots", .item_passive = bomb_trail, .description = "Become the bomber"};
}

Item construct_overclocking_module()
{
    return (Item){.textureID = 1, .name = "Overclocking module", .item_passive = attack_cooldown_decrease, .description = "Attack cooldown decreased"};
}

Item construct_multishot()
{
    return (Item){.textureID = 2, .name = "Cloning module", .item_passive = extra_bullet, .description = "Increases the number of projectiles shot per action"};
}

Item construct_attack_module()
{
    return (Item){.textureID = 3, .name = "Attack module", .item_passive = attack_damage_increase, .description = "Projectiles deal more damage"};
}

Item construct_bouncy_projectile()
{
    return (Item){.textureID = 4, .name = "Rebound module", .item_passive = bouncy_projectiles, .description = "Projectiles bounce of walls"};
}

Item construct_piercing()
{
    return (Item){.textureID = 5, .name = "Holographic module", .item_passive = piercing, .description = "Projectiles can now pass through enemies"};
}

Item construct_max_health()
{
    return (Item){.textureID = 6, .name = "Resilience module", .item_passive = max_health_increase, .description = "Maximum health increased"};
}

Item construct_speedy_gonzales()
{
    return (Item){.textureID = 8, .name = "Rocket boots", .item_passive = movement_speed_increase, .description = "Speed increased"};
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

Item construct_jetpack()
{
    return (Item){.textureID = 9, .name = "Jetpack", .item_passive = flight, .description = "Rule the skies"};
}