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
    player->entity->projectileStats.bounces = 2;
}

void piercing(Player *player)
{
    player->entity->projectileStats.pierces = 100;
}

void bomb_trail(Player *player)
{
    player->throws_mines = true;
}

Dialogue *construct_description(char *title, char *description)
{
    Dialogue *dialogue = calloc(1, sizeof(Dialogue));
    dialogue->title = title;
    dialogue->dialogueLines = calloc(1, sizeof(Dialogue *));
    *dialogue->dialogueLines = description;
    return dialogue;
}

Item construct_boom_boots()
{
    Dialogue *dialogue = calloc(1, sizeof(Dialogue));
    dialogue->title = "Boom Boots";
    dialogue->dialogueLines = calloc(1, sizeof(Dialogue *));
    *dialogue->dialogueLines = "Become the bomber";
    return (Item){.textureID = 0, .dialogue = construct_description("Boom Boots", "Become the bomber"), .item_passive = bomb_trail};
}

Item construct_overclocking_module()
{
    return (Item){.textureID = 1, .dialogue = construct_description("Overclocking module", "Attack cooldown decreased"), .item_passive = attack_cooldown_decrease};
}

Item construct_multishot()
{
    return (Item){.textureID = 2, .dialogue = construct_description("Cloning module", "Increases the number of projectiles shot per action"), .item_passive = extra_bullet};
}

Item construct_attack_module()
{
    return (Item){.textureID = 3, .dialogue = construct_description("Attack module", "Projectiles deal more damage"), .item_passive = attack_damage_increase};
}

Item construct_bouncy_projectile()
{
    return (Item){.textureID = 4, .dialogue = construct_description("Rebound module", "Projectiles bounce of walls"), .item_passive = bouncy_projectiles};
}

Item construct_piercing()
{
    return (Item){.textureID = 5, .dialogue = construct_description("Holographic module", "Projectiles can now pass through enemies"), .item_passive = piercing};
}

Item construct_max_health()
{
    return (Item){.textureID = 6, .dialogue = construct_description("Resilience module", "Maximum health increased"), .item_passive = max_health_increase};
}

Item construct_speedy_gonzales()
{
    return (Item){.textureID = 8, .dialogue = construct_description("Rocket boots", "Speed increased"), .item_passive = movement_speed_increase};
}

Item *construct_stopwatch()
{
    Item *stopwatch = malloc(sizeof(Item));
    *stopwatch = (Item){.textureID = 0, .item_active = time_travel, .active_cooldown = 300,
                        .dialogue = construct_description("Mysterious stopwatch", "Who knows?")};
    return stopwatch;
}

Item construct_jetpack()
{
    return (Item){.textureID = 9, .dialogue = construct_description("Jetpack", "Rule the skies"), .item_passive = flight};
}