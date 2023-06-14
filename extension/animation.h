#ifndef ANIMATION_H
#define ANIMATION_H

typedef unsigned int uint;
typedef struct Animation
{
    void *startVal, *endVal;
    uint framesPerUpdate, framesLeftUntilUpdate;
    void *curVal;
} Animation;

Animation* Animation_construct();


#endif //ANIMATION_H