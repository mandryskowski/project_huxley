#ifndef RENDER_H
#define RENDER_H
#include "state.h"
#include <stdbool.h>
#include <stddef.h>
#include "game_math.h"
typedef unsigned int uint;

typedef struct Mesh
{
    uint VAO;
    uint VBO;
    size_t vertexCount;
} Mesh;

typedef struct RenderState
{
    Mesh legacyGridMesh;
    Mesh isoMesh, isoMesh2;
    Mesh quadMesh;
    int shader;
    uint tileAtlas, characterAtlas, isoTileAtlas, isoCharacterAtlas, itemAtlas;

    bool renderIsometric;
    bool bDebugHitboxes;
    bool VSync;

    Vec2i resolution;

    Vec4d backgroundColor;

} RenderState;

RenderState RenderState_construct();

void initRenderState(GameState*, RenderState*);
void refreshRoom(GameState*, RenderState*);
Vec2d getIsoPos(Vec2d mapPos, Vec2i roomSize);

void render(GameState*, RenderState*);
void renderIsometric(GameState*, RenderState*);

#endif // RENDER_H