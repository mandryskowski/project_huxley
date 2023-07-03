#include "animation.h"
#include "entity.h"

// For NULL
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

Animation* Animation_construct()
{
    Animation* anim = malloc(sizeof(Animation));
    *anim = (Animation){.startVal = NULL, .endVal = NULL, .framesPerUpdate = 20, .framesLeftUntilUpdate = 20, .curVal = NULL};

    return anim;
}

bool animLogicInt(Animation* anim)
{
    if (--anim->framesLeftUntilUpdate > 0)
        return false;
    
    anim->framesLeftUntilUpdate = anim->framesPerUpdate;

    int* val = (int*)anim->curVal;
    int* start = (int*)anim->startVal;
    int* end = (int*)anim->endVal;

    //printf("val %f %f %f \n", *val, *start, *end);

    if (abs(*val - *end) < EPSILON)
    {
        *val = *start;
    }
    return true;
}


bool animLogicDouble(Animation* anim)
{
    if (--anim->framesLeftUntilUpdate > 0)
        return false;
    
    anim->framesLeftUntilUpdate = anim->framesPerUpdate;

    double* val = (double*)anim->curVal;
    double* start = (double*)anim->startVal;
    double* end = (double*)anim->endVal;

    //printf("val %f %f %f %f \n", *val, *start, *end, abs(*val - *end));

    if (fabs((*val) - (*end)) < EPSILON)
    {
        *val = *start;
    }
    return true;
}

bool mysterious_animation(Animation* anim)
{
    int* val = (int*)anim->curVal;
    if (animLogicInt(anim))
        (*val) += 2;
    return false;
}

bool portal_animation(Animation* anim)
{
   int* val = (int*)anim->curVal;
    if (animLogicInt(anim))
        (*val) += 1;
    return false;
}

Animation* Animation_construct_mysterious(Entity* entity)
{
    Animation* anim = Animation_construct();
    int *startVal = malloc(sizeof(int)), *endVal = malloc(sizeof(int));
    *startVal = 4;
    *endVal = 10;
    anim->curVal = &entity->textureID;
    *(int*)anim->curVal = *startVal;
    anim->startVal = startVal;
    anim->endVal = endVal; 
    anim->animFunc = mysterious_animation;
    entity->currentAnimation = anim;
    return anim;
}

Animation* Animation_construct_portal(Entity* entity)
{
    Animation* anim = Animation_construct();
    int *startVal = malloc(sizeof(int)), *endVal = malloc(sizeof(int));
    *startVal = 14;
    *endVal = 22;
    anim->curVal = &entity->textureID;
    *(int*)anim->curVal = *startVal;
    anim->startVal = startVal;
    anim->endVal = endVal; 
    anim->animFunc = portal_animation;
    anim->framesPerUpdate=5;
    entity->currentAnimation = anim;
    return anim;
}

void free_animation(Animation* anim)
{
    free(anim->startVal);
    free(anim->endVal);
    free(anim);
}
