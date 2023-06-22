#ifndef ENTITY_H
#define ENTITY_H

#include "game_math.h"
#include "room.h"
#include "queue.h"
#include "audio.h"
#include <stdbool.h>

typedef struct Item Item;
typedef enum ItemType ItemType;
typedef struct Entity Entity;
typedef struct Animation Animation;

typedef enum {
    ATTACK_CONTACT, SPAWN_ENTITY
} AttackType;

typedef enum {
    ALLY, ENEMY
} Faction;

typedef enum MonsterType {
    ZOMBIE, SHOOTER, FLYING_SHOOTER, NOT_MONSTER, BOMBER, HASKELL, MYSTERIOUS_CHARACTER, MINI_LAMBDA, PORTAL
} MonsterType;

typedef bool (*Attack_Func)(Entity *, Entity *, AttackType);
typedef void (*Death_Func)(Entity *);

typedef struct ProjectileStats
{
    int bounces;
    int pierces;
} ProjectileStats;

// This general class for Entity should ideally need just a few fields but the lack of OOP in C makes this hard to achieve.
// Shortcuts had to be used to obtain a minimum viable product within just 2 weeks.
typedef struct Entity
{
    Vec2d pos;
    Vec2d velocity;
    Vec2d attack_velocity;

    // relative to pos
    Rectangle hitbox;

    bool canFly;

    int HP, maxHP, attack_cooldown, cooldown_left;
    int ATK, attack_modifier;
    int hit_animation;
    double SPD, attack_SPD;

    ProjectileStats projectileStats;

    Faction faction;
    Attack_Func attack_func;
    Death_Func death_func;

    Room *room;
    Animation *currentAnimation; // can be NULL if not animated.

    uint textureID;
    Vec2d renderOffset; // for special animations like the boss jumping. Does not affect the hitbox so probably make the Entity invincible while this is happening.

    void* specific_data; // points to a Player object for player, a Boss object for a boss etc. Allows to retrieve extra fields in functions that accept Entity*.
} Entity;

typedef struct Dialogue
{
    char* title;
    char** dialogueLines;
    int dialogueSize;
    int dialogueIndex;
    double skipCooldown;
    bool isSkippable;

    Entity *creator;
} Dialogue;

typedef struct Npc
{
    Dialogue *dialogue;
} Npc;

typedef struct Player
{
    Entity *entity;
    double acceleration_const; // between 0 and 1.
    double movement_swing; // between 0 and 1.
    Vec2d cameraSize; // number of tiles visible on the x and y isometric diagonal centred around the player.

    Queue *prev_positions;

    bool throws_mines;
    
    bool isInDialogue;
    bool canEnterDialogue;
    double lastSkip;

    int screenShakeFramesLeft;
    double fadeToBlack; // alpha value for fade to black (0 means fully bright, 1 means fully black).

    int coins;
    int items_cnt;

    Item **items;
    Item *active_item;
} Player;


Entity *construct_monster(Vec2d pos, MonsterType type, Room *room);
Entity *construct_katsu(Vec2d pos, Room *room);
Entity *construct_coin(Vec2d pos, Room *room);
Entity *construct_item(ItemType itemType, Vec2d pos, int cost);
Player *Entity_construct_player();

bool isNotAMonster(Entity *entity);

bool isProjectile(Entity *);

bool isMine(Entity *);

bool isDead(Entity *);

bool isPickable(Entity *);

bool isKatsu(Entity *);

bool isNPC(Entity *);

bool isItem(Entity *);

bool isInteractable(Entity *);

void killEntity(Entity *);

void handle_attack(Entity *, Entity *, AttackType);

void shooter_spawn_attack(Entity *attacker);

void free_dialogue(Dialogue *dialogue);

void free_player(Player *player);
void free_entity(Entity *entity);

#endif // ENTITY_H