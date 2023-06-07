


typedef struct Entity
{
    Vec2i pos;
    Vec2f velocity;

    Vec2f hitboxBottomLeft;
    Vec2f hitboxTopRight;

    bool canFly;

    int HP, maxHP;
    float ATK;
    float SPD;

} Entity;

typedef struct Player
{
    Entity entity;
} Player;