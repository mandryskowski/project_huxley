#include "animation.h"

// For NULL
#include <stdio.h>
#include <stdlib.h>

Animation* Animation_construct()
{
    Animation* anim = malloc(sizeof(Animation));
    *anim = (Animation){.startVal = NULL, .endVal = NULL, .framesPerUpdate = 20, .framesLeftUntilUpdate = 20, .curVal = NULL};

    return anim;
}