#include "item.h"
#include "entity.h"
#include <stdlib.h>
#include <string.h>

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
    dialogue->title = calloc(1, strlen(title) + 1);
    strcpy(dialogue->title, title);

    dialogue->dialogueLines = calloc(1, sizeof(Dialogue *));

    *dialogue->dialogueLines = calloc(1, strlen(description) + 1);
    strcpy(*dialogue->dialogueLines, description);
    dialogue->dialogueSize = 1;
    return dialogue;
}

Item construct_boom_boots()
{
    return (Item){.textureID = 12, .dialogue = construct_description("Boom Boots", "Become the bomber"), .item_passive = bomb_trail};
}

Item construct_overclocking_module()
{
    return (Item){.textureID = 13, .dialogue = construct_description("Overclocking module", "Attack cooldown decreased"), .item_passive = attack_cooldown_decrease};
}

Item construct_multishot()
{
    return (Item){.textureID = 14, .dialogue = construct_description("Cloning module", "Increases the number of projectiles shot per action"), .item_passive = extra_bullet};
}

Item construct_attack_module()
{
    return (Item){.textureID = 15, .dialogue = construct_description("Attack module", "Projectiles deal more damage"), .item_passive = attack_damage_increase};
}

Item construct_bouncy_projectile()
{
    return (Item){.textureID = 8, .dialogue = construct_description("Rebound module", "Projectiles bounce of walls"), .item_passive = bouncy_projectiles};
}

Item construct_piercing()
{
    return (Item){.textureID = 9, .dialogue = construct_description("Holographic module", "Projectiles can now pass through enemies"), .item_passive = piercing};
}

Item construct_max_health()
{
    return (Item){.textureID = 10, .dialogue = construct_description("Resilience module", "Maximum health increased"), .item_passive = max_health_increase};
}

Item construct_speedy_gonzales()
{
    return (Item){.textureID = 4, .dialogue = construct_description("Rocket boots", "Speed increased"), .item_passive = movement_speed_increase};
}

Item construct_jetpack()
{
    return (Item){.textureID = 9, .dialogue = construct_description("Jetpack", "Rule the skies"), .item_passive = flight};
}

Item construct_stopwatch()
{
    return (Item){.textureID = 11, .item_active = time_travel, .active_cooldown = 300,
            .dialogue = construct_description("Mysterious hourglass", "Who knows?")};
}

Item *get_item(ItemType type){
    Item *item = calloc(1, sizeof(Item));
    switch (type) {
        case BOOM_BOOTS:
            *item = construct_boom_boots();
            break;
        case OVERCLOCKING_MODULE:
            *item = construct_overclocking_module();
            break;
        case CLONING_MODULE:
            *item = construct_multishot();
            break;
        case ATTACK_MODULE:
            *item = construct_attack_module();
            break;
        case REBOUND_MODULE:
            *item = construct_bouncy_projectile();
            break;
        case HOLOGRAPHIC_MODULE:
            *item = construct_piercing();
            break;
        case RESILIENCE_MODULE:
            *item = construct_max_health();
            break;
        case ROCKET_BOOTS:
            *item = construct_speedy_gonzales();
            break;
        case JETPACK:
            *item = construct_jetpack();
            break;
        case STOPWATCH:
            *item = construct_stopwatch();
    }

    return item;
}

Item *cpy_item(Item *item)
{
    Item *item_cpy = malloc(sizeof(Item));
    item_cpy->dialogue = cpy_dialogue(item->dialogue);
    item_cpy->textureID = item->textureID;
    item_cpy->item_passive = item->item_passive;
    item_cpy->item_active = item->item_active;
    item_cpy->cooldown_left = item->cooldown_left;
    item_cpy->active_cooldown = item->active_cooldown;

    return item_cpy;
}

void free_item(Item *item)
{
    free(item->dialogue);
    free(item);
}