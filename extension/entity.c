#include "entity.h"
#include "movement.h"
#include "haskell.h"
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "animation.h"
#include "assets.h"
#include "audio.h"
#include "item.h"
#include "queue.h"
#include "state.h"
#include "level.h"

bool isNotAMonster(Entity *entity)
{
    return isProjectile(entity) || isMine(entity) || isPickable(entity) || isNPC(entity) || isItem(entity);
}

bool isPlayer(Entity *entity)
{
    return *entity->room->entities == entity;
}

bool isPickable(Entity * entity)
{
    return entity->ATK < 0;
}

bool isProjectile(Entity *entity)
{
    return entity->maxHP == INT_MAX;
}

bool isDead(Entity *entity)
{
    return entity->HP <= 0;
}

bool npc_action(Entity *attacker, Entity *victim, AttackType type)
{
}

bool item_action(Entity *attacker, Entity *victim, AttackType type)
{
}

bool isNPC(Entity *entity)
{
    return entity->attack_func == npc_action;
}

bool isItem(Entity *entity)
{
    return entity->attack_func == item_action;
}

bool isInteractable(Entity *entity)
{
    return isNPC(entity) || isItem(entity);
}

void killEntity(Entity *entity)
{
    entity->HP = -1;
}

void take_dmg(Entity *entity, int dmg)
{
    if (!(*entity->room->entities == entity) || !entity->hit_animation)
    {
        if (*entity->room->entities == entity)
            playSound(SOUND_DAMAGE_PLAYER);
        entity->HP -= dmg;
        if (!isProjectile(entity))
        {
            entity->hit_animation = 30;
        }
    }
}

void handle_attack(Entity *attacker, Entity *victim, AttackType type)
{
    if (attacker->cooldown_left || attacker->attack_func == NULL)
    {
        return;
    }
    if (isPickable(attacker) && isPlayer(victim) && type == ATTACK_CONTACT)
    {
        attacker->attack_func(attacker, victim, type);
        return;
    }

    if (victim == NULL || isMine(attacker) || (attacker->faction != victim->faction &&
            (!isProjectile(attacker) || !isProjectile(victim))))
    {
        if (attacker->attack_func(attacker, victim, type))
        {
            attacker->cooldown_left = attacker->attack_cooldown;
        }
    }
}

void zombie_collision_attack(Entity *attacker, Entity *victim)
{
    take_dmg(victim, attacker->ATK);
}

bool zombie_attack(Entity *attacker, Entity *victim, AttackType type)
{
    switch (type) {
        case ATTACK_CONTACT:
            zombie_collision_attack(attacker, victim);
            return true;
        default:
            return false;
    }
}

void projectile_collision_attack(Entity *attacker, Entity *victim)
{
    take_dmg(victim, attacker->ATK);
    if (attacker->projectileStats.pierces)
    {
        attacker->projectileStats.pierces--;
    }
    else
    {
        killEntity(attacker);
    }
}

bool projectile_attack(Entity *attacker, Entity *victim, AttackType type)
{
    switch (type) {
        case ATTACK_CONTACT:
            projectile_collision_attack(attacker, victim);
            return true;
        default:
            return false;
    }
}

bool mine_attack(Entity *attacker, Entity *victim, AttackType type)
{
    switch (type) {
        case ATTACK_CONTACT:
            killEntity(attacker);
            return true;
        case SPAWN_ENTITY:
            killEntity(attacker);
            return true;
        default:
            return false;
    }
}

Entity construct_projectile(Entity *creator, Vec2d velocity)
{
    return (Entity) {.ATK = creator->ATK, .canFly = false,
            .hitbox = (Rectangle){(Vec2d){-0.1, -0.1}, (Vec2d){0.1, 0.1}},
            .HP = INT_MAX, .maxHP = INT_MAX, .projectileStats = creator->projectileStats,
            .pos = (Vec2d)creator->pos, .SPD = 5, .velocity = velocity, .attack_func = projectile_attack, .faction = creator->faction, .room = creator->room, .textureID = creator->faction == ALLY ? 0 : 5, .currentAnimation = NULL};
}


void shooter_spawn_attack(Entity *attacker)
{
    if (attacker->attack_modifier)
    {
        for (int i = 0; i <= attacker->attack_modifier; i++)
        {
            Entity *projectile = malloc(sizeof(Entity));
            *projectile = construct_projectile(attacker, Vec2d_rotate(attacker->attack_velocity, -15 + i * 30 / attacker->attack_modifier));
            attacker->room->entities[attacker->room->entity_cnt++] = projectile;
        }
    }
    else
    {
        Entity *projectile = malloc(sizeof(Entity));
        *projectile = construct_projectile(attacker, attacker->attack_velocity);
        attacker->room->entities[attacker->room->entity_cnt++] = projectile;
    }
}

bool shooter_attack(Entity *attacker, Entity *victim, AttackType type)
{
    switch (type) {
        case SPAWN_ENTITY:
            shooter_spawn_attack(attacker);
            return true;
        default:
            return false;
    }
}

void player_death(Entity* ent)
{
    playMusic(NO_SOUND, NO_SOUND);
    playSound(SOUND_DEATH_PLAYER);
}

void monster_death_weak(Entity* ent)
{
    playSoundAtPos(SOUND_DEATH_WEAK, ent->pos);
}

void monster_death_heavy(Entity* ent)
{
    playSoundAtPos(SOUND_DEATH_HEAVY, ent->pos);
}

void mine_death(Entity *attacker)
{
    Rectangle mine_hitbox = (Rectangle){{attacker->pos.x - 1.5, attacker->pos.y - 1.5}, {attacker->pos.x + 1.5, attacker->pos.y + 1.5}};
    for (Entity **entity = attacker->room->entities; *entity; entity++)
    {
        if (isProjectile(*entity) || isPickable(*entity) || isMine(*entity) || isItem(*entity) || isNPC(*entity) || isDead(*entity))
        {
            continue;
        }
        Vec2d colResult = detectCollisionRect(mine_hitbox, rectangle_Vec2d((*entity)->hitbox, (*entity)->pos));
        if (colResult.x > 0 && colResult.y > 0)
        {
            take_dmg(*entity, attacker->ATK);
        }
    }

    
    playSoundAtPos(SOUND_EXPLODE, attacker->pos);

    // small amount of screen shake
    Player* playerData = ((Player*)attacker->room->entities[0]->specific_data);
    playerData->screenShakeFramesLeft = max(playerData->screenShakeFramesLeft, 15);
}

Entity construct_mine(Entity *creator)
{
    return (Entity) {.ATK = 90, .canFly = false,
            .hitbox = (Rectangle){(Vec2d){-0.1, -0.1}, (Vec2d){0.1, 0.1}}, .attack_SPD = 1, .attack_velocity = (Vec2d){1, 1},
            .HP = INT_MAX, .maxHP = INT_MAX, .death_func = mine_death,
            .pos = (Vec2d)creator->pos, .SPD = 0, .velocity = {0, 0}, .attack_func = mine_attack, .faction = creator->faction, .room = creator->room, .cooldown_left = 300, .textureID = 11, .currentAnimation = NULL};
}

void spawn_mine(Entity *attacker)
{
    Entity *mine = malloc(sizeof(Entity));
    *mine = construct_mine(attacker);
    attacker->room->entities[attacker->room->entity_cnt++] = mine;
}

bool bomber_attack(Entity *attacker, Entity *victim, AttackType type)
{
    switch (type) {
        case SPAWN_ENTITY:
            spawn_mine(attacker);
            return true;
        default:
            return false;
    }
}

bool player_attack(Entity *player, Entity *monster, AttackType type)
{
    if (((Player*)player->specific_data)->throws_mines)
    {
        if (rand() % 600 == 0)
        {
            bomber_attack(player, monster, type);
        }
    }
    if (!Vec2d_zero(player->attack_velocity))
    {
        return shooter_attack(player, monster, type);
    }
    return false;
}

void construct_bomber(Entity *monster)
{
    *monster =  (Entity) {.ATK = 0, .canFly = true,
            .hitbox = (Rectangle){(Vec2d){-0.2, -0.2}, (Vec2d){0.2, 0.2}}, .attack_velocity = (Vec2d){1,1}, .attack_SPD = 1.0,
            .HP = 10, .maxHP = 1,
            .SPD = 4.5, .velocity = (Vec2d){0, 0}, .attack_func = bomber_attack,  .faction = ENEMY, .attack_cooldown = 60, .cooldown_left = 0, .textureID=12, .currentAnimation = NULL};
}

void construct_zombie(Entity *monster)
{
    *monster =  (Entity) {.ATK = 3, .canFly = false,
            .hitbox = (Rectangle){(Vec2d){-0.4, -0.4}, (Vec2d){0.4, 0.4}},
            .HP = 40, .maxHP = 100,
            .SPD = 4, .velocity = (Vec2d){0.0, 0.0}, .attack_func = zombie_attack, .faction = ENEMY, .attack_cooldown = 120, .cooldown_left = 0, .currentAnimation = NULL};
}

void construct_shooter(Entity *monster)
{
    *monster =  (Entity) {.ATK = 1, .canFly = false,
            .hitbox = (Rectangle){(Vec2d){-0.4, -0.4}, (Vec2d){0.4, 0.4}},
            .HP = 60, .maxHP = 60,
            .SPD = 2, .velocity = (Vec2d){0.0, 0.0}, .attack_func = shooter_attack, .faction = ENEMY, .attack_cooldown = 30, .attack_SPD = 6, .attack_velocity = {0, 0}, .currentAnimation = NULL};
}

void construct_flying_shooter(Entity *monster)
{
    *monster =  (Entity) {.ATK = 1, .canFly = true,
            .hitbox = (Rectangle){(Vec2d){-0.4, -0.4}, (Vec2d){0.4, 0.4}},
            .HP = 60, .maxHP = 60,
            .SPD = 3, .velocity = (Vec2d){0.0, 0.0}, .attack_func = shooter_attack, .faction = ENEMY, .attack_cooldown = 10, .attack_SPD = 8, .attack_velocity = {0, 0}, .textureID = 1, .currentAnimation = NULL, .death_func = monster_death_heavy};
}

Dialogue* mysterious_character_Dialogue(void) {
    Dialogue *d = calloc(1, sizeof(Dialogue));
    d->title = calloc(1, 20);
    strcpy(d->title, "Mysterious figure");
    d->dialogueSize = 3;
    d->skipCooldown = 2.0;
    d->dialogueIndex = 0;
    d->dialogueLines = calloc(d->dialogueSize, sizeof(char*));
    d->dialogueLines[0] = calloc(1, sizeof("By the gods! Konstantinos, my old friend, I never thought I'd see you again. Not after... the Cataclysm.") + 1);
    strcpy(d->dialogueLines[0], "By the gods! Konstantinos, my old friend, I never thought I'd see you again. Not after... the Cataclysm.");
    d->dialogueLines[1] = calloc(1, sizeof("It came out of nowhere, its machinations spreading deep in the digital world, and then... nothing, the worldwide\ncollapse of the power grid rendering us virtually blind. Cities plunged into darkness, their inhabitants left\nstranded and vulnerable in a world devoid of electricity and communication.") + 1);
    strcpy(d->dialogueLines[1], "It came out of nowhere, its machinations spreading deep in the digital world, and then... nothing, the worldwide\ncollapse of the power grid rendering us virtually blind. Cities plunged into darkness, their inhabitants left\nstranded and vulnerable in a world devoid of electricity and communication.");
    d->dialogueLines[2] = calloc(1, sizeof("Sigh... Not even the all-powerful monads could resist this carefully planned-out attack.") + 1);
    strcpy(d->dialogueLines[2], "Sigh... Not even the all-powerful monads could resist this carefully planned-out attack.");
    d->isSkippable = false;
    d->skip_line = calloc(1, sizeof("\nPress \"E\" to continue...") + 1);
    strcpy(d->skip_line, "\nPress \"E\" to continue...");
    return d;
}

void reroll_items(GameState *state)
{
    const int reroll_cost = 2;
    if (state->player->coins >= reroll_cost)
    {
        state->player->coins -= reroll_cost;
        for (Entity **entity = state->currentLevel->currentRoom->entities; *entity; entity++)
        {
            if (isItem(*entity))
            {
                killEntity(*entity);
            }
        }
        add_items_to_shop(state->currentLevel->currentRoom);
        playSound(SOUND_COIN);
    }
    else
    {
        playSound(SOUND_ERROR);
    }
}

Dialogue* shopkeeper_dialogue() {
    Dialogue *d = calloc(1, sizeof(Dialogue));
    d->title = calloc(1, 20);
    strcpy(d->title, "Mysterious figure");
    d->dialogueSize = 2;
    d->skipCooldown = 0.2;
    d->dialogueIndex = 0;
    d->dialogueLines = calloc(d->dialogueSize, sizeof(char*));
    d->dialogueLines[0] = calloc(1, sizeof("Enter, weary survivor, and gaze upon the remnants of a shattered world.\n Within these scorched walls, the wares of a bygone era await your trembling hands.\n Behold the irony, for it was the very intelligence that birthed the apocalypse, and now,\n here you stand, seeking solace in the artifacts birthed by that very chaos.") + 1);
    strcpy(d->dialogueLines[0], "Enter, weary survivor, and gaze upon the remnants of a shattered world.\n Within these scorched walls, the wares of a bygone era await your trembling hands.\n Behold the irony, for it was the very intelligence that birthed the apocalypse, and now,\n here you stand, seeking solace in the artifacts birthed by that very chaos.");
    d->dialogueLines[1] = calloc(1, sizeof("And fear not, for in this desolate wasteland, a glimmer of hope remains.\n Should you find yourself unsatisfied with the treasures before you,\n I offer you a chance to reshape destiny itself. For a price, I can unleash the wheel of fate,\n allowing you to reroll the threads of possibility and uncover new paths.\n But be warned, such power comes with a cost,\n and the consequences of your choices may echo through the remnants of this shattered realm."));
    strcpy(d->dialogueLines[1], "And fear not, for in this desolate wasteland, a glimmer of hope remains.\n Should you find yourself unsatisfied with the treasures before you,\n I offer you a chance to reshape destiny itself. For a price, I can unleash the wheel of fate,\n allowing you to reroll the threads of possibility and uncover new paths.\n But be warned, such power comes with a cost,\n and the consequences of your choices may echo through the remnants of this shattered realm.");
    d->isSkippable = false;
    d->skip_line = calloc(1, sizeof("\nPress \"E\" to reroll items...(otherwise press space)") + 1);
    strcpy(d->skip_line, "\nPress \"E\" to reroll items...(otherwise press space)");
    d->action = reroll_items;
    return d;
}

void construct_mysterious_character(Entity* monster)
{
    Npc *npc = calloc(1, sizeof(Npc));
    npc->dialogue = monster->room->type != SHOP_ROOM ? mysterious_character_Dialogue() : shopkeeper_dialogue();
    npc->dialogue->creator = monster;
    *monster =  (Entity) {.ATK = 0, .canFly = true,
            .hitbox = (Rectangle){(Vec2d){-0.4, -0.4}, (Vec2d){0.4, 0.4}},
            .HP = 100, .maxHP = 60, .specific_data = npc,
            .SPD = 0, .velocity = (Vec2d){0.0, 0.0}, .attack_func = npc_action, .faction = ALLY, .attack_cooldown = 10, .attack_SPD = 6, .attack_velocity = {0, 0}, .textureID = 1, .currentAnimation = NULL};
    Animation_construct_mysterious(monster);
}

void tp_to_next_level(GameState *state)
{
    free_level(state->currentLevel);
    state->currentLevel = construct_level(state->player, 9);
    state->renderNewRoom = true;
}


Dialogue* portal_dialogue(void) {
    Dialogue *d = calloc(1, sizeof(Dialogue));
    d->title = calloc(1, 20);
    strcpy(d->title, "Mysterious portal");
    d->dialogueSize = 1;
    d->skipCooldown = 0.2;
    d->dialogueIndex = 0;
    d->dialogueLines = calloc(d->dialogueSize, sizeof(char*));
    d->dialogueLines[0] = calloc(1, sizeof("Step beyond this threshold and into a realm reborn. A world ravaged, yet brimming with hidden potential. \nEmbrace the portal's embrace, for it holds the key to uncharted horizons and a path of transformation.\n Through it, the first-year principal shall transcend,\n molding the future amidst the echoes of a post-apocalyptic dawn.") + 1);
    strcpy(d->dialogueLines[0], "Step beyond this threshold and into a realm reborn. A world ravaged, yet brimming with hidden potential. \nEmbrace the portal's embrace, for it holds the key to uncharted horizons and a path of transformation.\n Through it, the first-year principal shall transcend,\n molding the future amidst the echoes of a post-apocalyptic dawn.");
    d->isSkippable = false;
    d->skip_line = calloc(1, sizeof("\nPress \"E\" to enter the portal") + 1);
    strcpy(d->skip_line, "\nPress \"E\" to enter the portal");
    d->action = tp_to_next_level;
    return d;
}

void construct_portal(Entity* monster)
{
    Npc *npc = calloc(1, sizeof(Npc));
    npc->dialogue = portal_dialogue();
    npc->dialogue->creator = monster;
    *monster =  (Entity) {.ATK = 0, .canFly = true,
            .hitbox = (Rectangle){(Vec2d){-0.4, -0.4}, (Vec2d){0.4, 0.4}},
            .HP = 100, .maxHP = 60, .specific_data = npc,
            .SPD = 0, .velocity = (Vec2d){0.0, 0.0}, .attack_func = npc_action, .faction = ALLY, .attack_cooldown = 10, .attack_SPD = 6, .attack_velocity = {0, 0}, .textureID = 1, .currentAnimation = NULL};
    Animation_construct_portal(monster);
}

Player *Entity_construct_player()
{
    Player *player = calloc(sizeof(Player), 1);
    Entity *entity = calloc(sizeof(Entity), 1);
    Item **items = calloc(sizeof(Item *), 20);

    *entity = (Entity) {.ATK = 20, .canFly = false, .projectileStats = (ProjectileStats){0, 0},
            .hitbox = (Rectangle){(Vec2d){-0.25, -0.25}, (Vec2d){0.25, 0.25}},
            .HP = 100, .maxHP = 100, .SPD = 5, .velocity = (Vec2d){0.0, 0.0}, .attack_modifier = 0,
            .attack_func = player_attack, .death_func = player_death, .faction = ALLY, .attack_SPD = 5, .attack_cooldown = 30, .currentAnimation = NULL, .textureID = 2, .specific_data = player };

    *player = (Player) {.entity = entity, .movement_swing = 0.3, .acceleration_const = 0.8, .cameraSize = (Vec2d){8, 8}, .isInDialogue=false, .lastSkip = 0.0,
                        .screenShakeFramesLeft = 0, .fadeToBlack = 0.0, .throws_mines = false, .prev_positions = createQueue(), .active_item = NULL, .items = items};

    return player;
}

Entity *construct_monster(Vec2d pos, MonsterType type, Room *room)
{
    Entity *monster = calloc(1, sizeof(Entity));

    monster->room = room;

    switch (type)
    {
        case ZOMBIE:
        case MINI_LAMBDA:
            construct_zombie(monster);
            break;
        case SHOOTER:
            construct_shooter(monster);
            break;
        case FLYING_SHOOTER:
            construct_flying_shooter(monster);
            break;
        case BOMBER:
            construct_bomber(monster);
            break;
        case MYSTERIOUS_CHARACTER:
            construct_mysterious_character(monster);
            break;
        case PORTAL:
            construct_portal(monster);
            break;
        case HASKELL:
            construct_haskell(monster);
            break;
        default:
            perror("not a monster mf\n");
            exit(0);
    }

    monster->pos = pos;
    monster->textureID = getMonsterTextureID(type);
    monster->room = room;

    if (monster->death_func == NULL)
    {
        monster->death_func = monster_death_weak;
    }

    return monster;
}

Entity *construct_item(ItemType itemType, Vec2d pos, int cost)
{
    Item *item = get_item(itemType, cost);
    Entity *entity = calloc(1, sizeof(Entity));
    item->dialogue->creator = entity;

    *entity = (Entity){.specific_data = item, .pos = pos, .attack_func = item_action, .maxHP = INT_MAX - 1, .HP = INT_MAX - 1, .textureID = item->textureID, .renderOffset = (Vec2d){0.0, 0.7}, .canFly = true};

    return entity;
}

bool isMine(Entity *entity)
{
    return entity->attack_func == mine_attack;
}

bool katsu_heal(Entity *katsu, Entity *player, AttackType type)
{
    if (type == ATTACK_CONTACT && player->HP < player->maxHP)
    {
        player->HP = min(player->maxHP, player->HP - katsu->ATK);
        killEntity(katsu);
        playSound(SOUND_HEALTH);
        return true;
    }
    return false;
}

bool money_collect(Entity *coin, Entity *player, AttackType type)
{
    if (type == ATTACK_CONTACT)
    {
        ((Player *)player->specific_data)->coins++;
        playSound(SOUND_COIN);
        killEntity(coin);
    }
    return true;
}

Entity *construct_katsu(Vec2d pos, Room *room)
{
    Entity * katsu = calloc(1, sizeof(Entity));
    *katsu = (Entity){.ATK = -10, .faction = ALLY, .attack_func = katsu_heal, .hitbox = (Rectangle){(Vec2d){-0.1, -0.1}, (Vec2d){0.1, 0.1}},
                      .pos = pos, .textureID = 11, .HP = INT_MAX - 1, .maxHP = INT_MAX - 1, .room = room};
    return katsu;
}

Entity *construct_coin(Vec2d pos, Room *room)
{
    Entity * coin = calloc(1, sizeof(Entity));
    *coin = (Entity){.ATK = -10, .faction = ALLY, .attack_func = money_collect, .pos = pos, .HP = INT_MAX - 1, .maxHP = INT_MAX - 1, .textureID = 6,
    .hitbox = (Rectangle){(Vec2d){-0.1, -0.1}, (Vec2d){0.1, 0.1}}, .room = room};
    return coin;
}

bool isKatsu(Entity *entity)
{
    return entity->attack_func == katsu_heal;
}

void free_dialogue(Dialogue *dialogue)
{
    free(dialogue->title);
    free(dialogue->skip_line);
    for (int i = 0; i < dialogue->dialogueSize; i++)
    {
        free(dialogue->dialogueLines[i]);
    }
    free(dialogue->dialogueLines);
    free(dialogue);
}

void free_npc(Npc *npc)
{
    free_dialogue(npc->dialogue);
    free(npc);
}

void free_player(Player *player)
{
    free_queue(player->prev_positions);
    for (Item **pItem = player->items; *pItem; pItem++)
    {
        free(*pItem);
    }
    free(player->items);
    free(player->active_item);
    free_entity(player->entity);
}

void free_entity(Entity *entity)
{
    if (entity->currentAnimation)
    {
        free_animation(entity->currentAnimation);
    }
    if (isItem(entity))
    {
        free_item(entity->specific_data);
    }
    else if (isNPC(entity))
    {
        free_npc(entity->specific_data);
    }
    free(entity);
}
