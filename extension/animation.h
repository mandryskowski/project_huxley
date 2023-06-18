#ifndef ANIMATION_H
#define ANIMATION_H

#include <stdbool.h>

typedef unsigned int uint;
typedef struct Entity Entity;
typedef struct Animation Animation;


typedef bool (*Animation_Func)(Animation*);

typedef struct Animation
{
    void *startVal, *endVal;
    uint framesPerUpdate, framesLeftUntilUpdate;
    void *curVal;

    Animation_Func animFunc;
} Animation;

Animation* Animation_construct();

bool animLogicInt(Animation* anim);
bool animLogicDouble(Animation* anim);

Animation* Animation_construct_mysterious(Entity* entity);


#endif //ANIMATION_H