#ifndef RENDER_H
#define RENDER_H
#include "state.h"
#include <stdbool.h>
#include "game_math.h"
typedef unsigned int uint;

typedef struct RenderState
{
    uint LevelVAO;
    uint IsoLevelVAO, IsoLevelVAO2;
    uint QuadVAO;
    int shader;
    uint tileAtlas, characterAtlas, isoTileAtlas, isoCharacterAtlas;

    bool renderIsometric;
    bool bDebugHitboxes;
    bool VSync;

    Vec2i resolution;

} RenderState;

RenderState RenderState_construct();

void initRenderState(GameState*, RenderState*);
void refreshRoom(GameState*, RenderState*);

void render(GameState*, RenderState*);
void renderIsometric(GameState*, RenderState*);

#endif // RENDER_H