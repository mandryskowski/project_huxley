#ifndef RENDER_H
#define RENDER_H
#include "state.h"
typedef unsigned int uint;

typedef struct RenderState
{
    uint LevelVAO;
    uint QuadVAO;
    int shader;
    uint tileAtlas, characterAtlas;
} RenderState;

void initRenderState(GameState*, RenderState*);

void render(GameState*, RenderState*);

#endif // RENDER_H