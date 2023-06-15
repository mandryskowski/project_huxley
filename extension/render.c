#include "render.h"
#include "glad/glad.h"
#include "glfw/glfw3.h"
#include "game_math.h"
#include "room.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "entity.h"
#include "assets.h"
#include "level.h"
#include "math.h"

Mesh Mesh_construct()
{
    return (Mesh){.VAO = 0, .VBO = 0, .vertexCount = 0};
}

RenderState RenderState_construct()
{
    return (RenderState){.bDebugHitboxes = false, .VSync = false, .renderIsometric = true,
                         .characterAtlas = loadAtlas("character.png", 1, 1), .legacyGridMesh = Mesh_construct(), .isoMesh = Mesh_construct(), .isoMesh2 = Mesh_construct(), .quadMesh = Mesh_construct(), .shader = 0, .tileAtlas = loadAtlas("textures.png", 1, 4),
                         .isoTileAtlas = loadAtlas("isoatlas.png", 4, 4), .isoCharacterAtlas = loadAtlas("isocharacter.png", 2, 6),
                         .resolution = (Vec2i){2048, 2048}, .backgroundColor = (Vec4d){0.2, 0.2, 0.2, 1.0}};
}

uint getTileTextureID(TileType type)
{
    switch (type)
    {
        case TILE_FLOOR: return 15 - 3;
        case TILE_WALL: return 15 - 1;
        case TILE_BARRIER: return 15;
        case TILE_HOLE: return 15 - 7;
        case TILE_DOOR: return getTileTextureID(TILE_FLOOR);
        default: return 0;
    }
}

bool isTileTypeOnFloor(TileType type)
{
    switch (type)
    {
        case TILE_BARRIER:
        case TILE_HOLE:
        case TILE_WALL:
            return true;
        default:
            return false;
    }

}


const char vShaderSrc[] = "#version 400 core\n" \
                      "layout (location = 0) in vec2 vPosition;\n" \
                      "layout (location = 1) in vec2 vTexCoord;\n" \
                      "layout (location = 2) in uint vTexID;\n" \
                      "layout (location = 3) in uint vDepth;\n" \
                      "flat out uint fsTexID;\n" \
                      "out vec2 fsTexCoord;\n" \
                      "uniform mat3 viewMat;\n" \
                      "uniform int customDepth;\n" \
                      "void main() \n" \
                      "{ \n" \
                      " float depth = (customDepth == -1) ? (float(vDepth)) : (float(customDepth));\n" \
                      " gl_Position = vec4(vec2(viewMat * vec3(vPosition, 1.0)), depth / 255.0, 1.0);\n" \
                      " fsTexID = vTexID;\n" \
                      " fsTexCoord = vTexCoord;\n" \
                      "}\0";


const char fShaderSrc[] = "#version 400 core\n" \
                      "flat in uint fsTexID;\n" \
                      "in vec2 fsTexCoord;\n" \
                      "out vec4 outColor;\n" \
                      "uniform sampler2DArray atlas;\n" \
                      "uniform int materialType;\n" \
                      "uniform int unTexId;\n"\
                      "uniform int flipHorizontal; \n" \
                      "uniform vec4 unColor;\n" \
                      "uniform float shadowSize;\n" \
                      "void main() \n" \
                      "{ \n" \
                      "vec4 color = vec4(0.0);\n" \
                      "vec2 texCoord = fsTexCoord;\n"\
                      "float discardThreshold = 0.5;\n"\
                      "if (flipHorizontal != 0) { texCoord.x = 1.0 - texCoord.x; }\n" \
                      "if (materialType == 0) { \n" \
                      " color = texture(atlas, vec3(texCoord, fsTexID));\n" \
                      "} else if (materialType == 1) { \n" \
                      " color = unColor;\n" \
                      "} else if (materialType == 2 || materialType == 4) {\n" \
                      " color = texture(atlas, vec3(texCoord, unTexId));  \n" \
                      "} else if (materialType == 5) { \n" \
                      " color = texture(atlas, vec3(texCoord, fsTexID)) * unColor;\n" \
                      "} else if (materialType == 6) { \n" \
                      " discardThreshold = 0.0; color = vec4(0.0, 0.0, 0.0, 0.5 * pow(1.0 - distance(vec2(0.5), texCoord), shadowSize));\n  " \
                      "} if (color.a < discardThreshold) \n" \
                      "{\n" \
                      " discard;\n" \
                      "} \n" \
                      "outColor = color;\n" \
                      "if (materialType == 4) outColor = unColor;\n" \
                      "//outColor = vec4(texCoord, 0.0, 1.0);\n"\
                      "}";

void getShadowInfo(Entity* ent, Vec2d* shadowCenterOffset, Vec2d* modelOffset, float* shadowSize)
{
    if (ent->canFly)
    {
        *shadowCenterOffset = (Vec2d){0.0, -0.35};
        double oscillation = (sin(glfwGetTime() * 3.0) * 0.5 + 0.5);
        *shadowSize =  oscillation * 8.0 + 8.0;
        modelOffset->y = (-oscillation * 0.1) * 0.9 + 0.1;
        return;
    }

    *shadowCenterOffset = (Vec2d){0.0, -0.35};
    *shadowSize = 25;
}

typedef struct Vertex
{
    Vec2d position;
    Vec2d texCoord;
    int textureID;
    int tileDepth;
} Vertex;

Mesh initGridMesh(GameState* gameState, Vertex* verts, uint width, uint height)
{ 
    Vec2d offsets[] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f},  // bottom right triangle
                       {0.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}}; // top left triangle
    Mesh outMesh = Mesh_construct();
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            for (int i = 0; i < 6; i++)
            {
                *verts = (Vertex){.position = Vec2d_add(offsets[i], (Vec2d){x, y}),
                                  .texCoord = offsets[i],
                                  .textureID = gameState->currentLevel->currentRoom->tiles[x][y].textureID,
                                  .tileDepth = 0
                                  };
                outMesh.vertexCount++;
                                  //.textureID = y * 16 + x};
                //printf("vert %f %f \n", verts->position.x, verts->position.y);
                verts++;
            }
        }
    }

    return outMesh;
}

Vec2d getIsoPos(Vec2d mapPos, Vec2i roomSize)
{
        float mulx = 0.39 * 2, muly = 0.39;
    //width = 2;
   // height = 2;
    Vec2d tileSize = (Vec2d){1.0, 1.0};
    Vec2d xOffset = (Vec2d){tileSize.x * -mulx, tileSize.y * -muly};
    Vec2d yOffset = (Vec2d){tileSize.x * mulx , -tileSize.y * muly};

    return Vec2d_add(Vec2d_scale(xOffset, roomSize.x - 0.5 - mapPos.x), 
                     Vec2d_scale(yOffset, roomSize.y - 0.5 - mapPos.y));
}

Vec2d getIsoVec(Vec2d vec)
{
            float mulx = 0.39 * 2, muly = 0.39;
    //width = 2;
   // height = 2;
    Vec2d xOffset = (Vec2d){-mulx, -muly};
    Vec2d yOffset = (Vec2d){mulx , -muly};

    return Vec2d_add(Vec2d_scale(xOffset, vec.x), 
                     Vec2d_scale(yOffset, vec.y));
}

Mesh initIsoMesh(GameState* gameState, Vertex* verts, uint width, uint height)
{
    Vec2d offsets[] = {{-1.0, -1.0}, {1.0, -1.0}, {1.0, 1.0},  // bottom right triangle
                       {-1.0, -1.0}, {1.0, 1.0}, {-1.0, 1.0}}; // top left triangle
    float mulx = 0.39 * 2, muly = 0.39;
    //width = 2;
   // height = 2;
    Vec2d tileSize = (Vec2d){1.0, 1.0};
    Vec2d xOffset = (Vec2d){tileSize.x * -mulx, tileSize.y * -muly};
    Vec2d yOffset = (Vec2d){tileSize.x * mulx , -tileSize.y * muly};

    Mesh outMesh = Mesh_construct();

    for (int x = width - 1; x >= 0; x--)
    {
        for (int y = height - 1; y >= 0; y--)
        {
            const Vec2d quadPos = Vec2d_add(Vec2d_scale(xOffset, (width - 1 - x)), 
                                            Vec2d_scale(yOffset, (height - 1 - y)));
            TileType type = gameState->currentLevel->currentRoom->tiles[x][y].type;

            int texID = getTileTextureID(type);

            if (isTileTypeOnFloor(type))
            {
                for (int i = 0; i < 6; i++)
            {
                int tileDepth = (x + y) + 1;

                *verts = (Vertex){.position = Vec2d_add((Vec2d){offsets[i].x * tileSize.x, offsets[i].y * tileSize.y}, quadPos),
                                  .texCoord = Vec2d_add((Vec2d){0.5, 0.5}, Vec2d_scale(offsets[i], 0.5)),
                                  .textureID =  getTileTextureID(TILE_FLOOR),
                                  .tileDepth = tileDepth + ((type == TILE_WALL) ?  0 : 1)};
                outMesh.vertexCount++;
                                  //.textureID = gameState->currentLevel->currentRoom->tiles[x][y].textureID};
                                  //.textureID = y * 16 + x};
                //printf("tcoord %f %f\n", verts->texCoord.x, verts->texCoord.y);
                verts++;
            }

            }

            for (int i = 0; i < 6; i++)
            {
                int tileDepth = (x + y) + 1;

                *verts = (Vertex){.position = Vec2d_add((Vec2d){offsets[i].x * tileSize.x, offsets[i].y * tileSize.y}, quadPos),
                                  .texCoord = Vec2d_add((Vec2d){0.5, 0.5}, Vec2d_scale(offsets[i], 0.5)),
                                  .textureID = texID,
                                  .tileDepth = tileDepth + ((type == TILE_WALL) ?  0 : 1)};
                outMesh.vertexCount++;
                                  //.textureID = gameState->currentLevel->currentRoom->tiles[x][y].textureID};
                                  //.textureID = y * 16 + x};
                //printf("tcoord %f %f\n", verts->texCoord.x, verts->texCoord.y);
                verts++;
            }
           // printf("vert (%d %d) %f %f \n", x, y,quadPos.x, quadPos.y);
        }
    }

    //printf("out mesh has %d verts\n", outMesh.vertexCount);
    return outMesh;
}

void debugShader(GLint shader)
{
    int success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == 0)
    {
        char *debugBuf = malloc(2048);
        memset(debugBuf, '\0', 2048);
        glGetShaderInfoLog(shader, 2048, NULL, debugBuf);
       // debugBuf[2047] = '\0';

        printf("Shader error: %s\n", debugBuf);
    }
}

void passMeshToGPU(Mesh* mesh, void* data)
{
    size_t size = mesh->vertexCount * sizeof(Vertex);

    glGenBuffers(1, &mesh->VBO);
    glGenVertexArrays(1, &mesh->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

    glBindVertexArray(mesh->VAO);
    glVertexAttribPointer(0, 2, GL_DOUBLE, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glVertexAttribPointer(1, 2, GL_DOUBLE, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
    glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, sizeof(Vertex), (void*)offsetof(Vertex, textureID));
    glVertexAttribIPointer(3, 1, GL_UNSIGNED_INT, sizeof(Vertex), (void*)offsetof(Vertex, tileDepth));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
}

void refreshRoom(GameState* gState, RenderState* rState)
{
    // Grid
    {
        if (rState->legacyGridMesh.vertexCount != 0)
        {
            glDeleteBuffers(1, &rState->legacyGridMesh.VBO);
            glDeleteVertexArrays(1, &rState->legacyGridMesh.VAO);
        }
        Vertex gridVerts[gState->currentLevel->currentRoom->size.x * gState->currentLevel->currentRoom->size.y * 6];
        rState->legacyGridMesh = initGridMesh(gState, gridVerts, gState->currentLevel->currentRoom->size.x, gState->currentLevel->currentRoom->size.y);
        passMeshToGPU(&rState->legacyGridMesh, gridVerts);
    }

    //  ISO Grid
    {

        if (rState->isoMesh2.vertexCount != 0)
        {
            glDeleteBuffers(1, &rState->isoMesh2.VBO);
            glDeleteVertexArrays(1, &rState->isoMesh2.VAO);
        }
        rState->isoMesh2 = rState->isoMesh;
        
        Vertex gridVerts[gState->currentLevel->currentRoom->size.x * gState->currentLevel->currentRoom->size.y * 6 * 2];
        rState->isoMesh = initIsoMesh(gState, gridVerts, gState->currentLevel->currentRoom->size.x, gState->currentLevel->currentRoom->size.y);
        passMeshToGPU(&rState->isoMesh, gridVerts);
    }
}

void initRenderState(GameState* gState, RenderState* rState)
{
    refreshRoom(gState, rState);

    // Quad
    {
        Vertex quadVerts[6];
        
        Vec2d offsets[] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f},  // bottom right triangle
                       {0.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}}; // top left triangle
        for (int i = 0; i < 6; i++)
        {
            quadVerts[i] = (Vertex){{offsets[i].x * 2.0 - 1.0, offsets[i].y * 2.0 - 1.0},
                            offsets[i],
                            0,
                            0};
            rState->quadMesh.vertexCount++;
        }

        passMeshToGPU(&rState->quadMesh, quadVerts);
    }

        
    printf("len %d \n", strlen(vShaderSrc));
    uint vShader = glCreateShader(GL_VERTEX_SHADER);

    {
        char *pbuffer = malloc(2048);
        strcpy(pbuffer, vShaderSrc);
        glShaderSource(vShader, 1, &pbuffer, NULL);
        glCompileShader(vShader);
        debugShader(vShader);
    }
    uint fShader = glCreateShader(GL_FRAGMENT_SHADER);
    {
        char *pbuffer = malloc(2048);
        strcpy(pbuffer, fShaderSrc);
        glShaderSource(fShader, 1, &pbuffer, NULL);
        glCompileShader(fShader);
        debugShader(fShader);
    }

    rState->shader = glCreateProgram();

    glAttachShader(rState->shader, vShader);
    glAttachShader(rState->shader, fShader);

    glLinkProgram(rState->shader);
    glUseProgram(rState->shader);

    glDetachShader(rState->shader, vShader);
    glDetachShader(rState->shader, fShader);

    glDeleteShader(vShader);
    glDeleteShader(fShader);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_MULTISAMPLE); 
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(rState->shader, "atlas"), 0);
}

Mat3f renderGrid(GameState* gState, RenderState* state)
{
    Mat3f viewMat = Mat3f_construct((Vec2d){-0.5f * gState->currentLevel->currentRoom->size.x / gState->currentLevel->currentRoom->size.y, -0.5f}, (Vec2d){1.0f / gState->currentLevel->currentRoom->size.y, 1.0f / gState->currentLevel->currentRoom->size.y});
    glUniformMatrix3fv(glGetUniformLocation(state->shader, "viewMat"), 1, GL_FALSE, viewMat.d);
    
    glBindTexture(GL_TEXTURE_2D_ARRAY, state->tileAtlas);
    glBindVertexArray(state->legacyGridMesh.VAO);
    glDrawArrays(GL_TRIANGLES, 0, gState->currentLevel->currentRoom->size.x * gState->currentLevel->currentRoom->size.y * 6);
    return viewMat;
}

Mat3f renderIsoGrid(GameState* gState, RenderState* state, Mesh* gridMesh, Vec2d offset)
{
        Vec2d scaledSize = (Vec2d){1.282 / ((gState->player->cameraSize.x)), 1.282 * ((double)state->resolution.x / (double)state->resolution.y) / ((gState->player->cameraSize.y))};
    Vec2d lolOffset = (Vec2d){0,0};// Vec2d_add(Vec2d_scale(gState->player->cameraSize, -0.19), (Vec2d){5.94, 5.94});
    Vec2d cameraCenterGrid = getIsoPos(Vec2d_add(offset,Vec2d_add(gState->player->entity->pos, Vec2d_rotate(Vec2d_scale(gState->player->cameraSize, 0), 0))), gState->currentLevel->currentRoom->size);
    //*viewMat = Mat3f_construct( (Vec2d){ -cameraCenterGrid.x * (0.19 * gState->player->cameraSize.x / gState->currentRoom->size.y),  -cameraCenterGrid.y* (0.19 * gState->player->cameraSize.y / gState->currentRoom->size.y)}, (Vec2d){lolOffset.x / gState->currentRoom->size.y, lolOffset.y / gState->currentRoom->size.y});
    //*viewMat = Mat3f_construct( (Vec2d){ -cameraCenterGrid.x * (5.0 / gState->currentRoom->size.y),  -cameraCenterGrid.y* (5.0 / gState->currentRoom->size.y)}, (Vec2d){5.0f / gState->currentRoom->size.y, 5.0f / gState->currentRoom->size.y});
    Mat3f viewMat = Mat3f_construct( (Vec2d){ -cameraCenterGrid.x * scaledSize.x,  -cameraCenterGrid.y* scaledSize.y}, scaledSize);
    //*viewMat = Mat3f_construct( (Vec2d){ -cameraCenterGrid.x * 0.05,  -cameraCenterGrid.y* 0.05}, (Vec2d){1.0 / (gState->player->cameraSize.x) *0.055, (1.0 / gState->player->cameraSize.y) * 0.055});
    //*viewMat = Mat3f_construct(getIsoPos(Vec2d_scale(Vec2i_to_Vec2d(gState->currentRoom->size), 0.5), gState->currentRoom->size), (Vec2d){1.0f / gState->currentRoom->size.y, 1.0f / gState->currentRoom->size.y});
    glUniformMatrix3fv(glGetUniformLocation(state->shader, "viewMat"), 1, GL_FALSE, viewMat.d);
    if (gridMesh == &state->isoMesh2 && 0)
        {
            const double fadeDistance = 10.0;
            double fadeVal;
            if (offset.x < 0) fadeVal = 1.0 - (gState->currentLevel->currentRoom->size.x - gState->player->entity->pos.x) / fadeDistance;
            else if (offset.x > 0) fadeVal = 1.0 - gState->player->entity->pos.x / fadeDistance;
            else if (offset.y < 0) fadeVal = 1.0 - (gState->currentLevel->currentRoom->size.y - gState->player->entity->pos.y) / fadeDistance;
            else fadeVal = 1.0 - gState->player->entity->pos.y / fadeDistance;
           // printf ("fade %f %f \n", offset.x, offset.y);
            Vec4f fadeCol = Vec4d_to_Vec4f(Vec4d_lerp(state->backgroundColor, (Vec4d){1.0, 1.0, 1.0, 1.0}, min(1.0, fadeVal)));
           // printf ("fadecol %f %f %f %f \n", fadeCol.x, fadeCol.y, fadeCol.z, fadeCol.w);
            glUniform4fv(glGetUniformLocation(state->shader, "unColor"), 1, &fadeCol);
            glUniform1i(glGetUniformLocation(state->shader, "materialType"), 5);
        }
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  
    glBindTexture(GL_TEXTURE_2D_ARRAY, state->isoTileAtlas);
    glBindVertexArray(gridMesh->VAO);
    glDrawArrays(GL_TRIANGLES, 0, gridMesh->vertexCount);
    glUniform1i(glGetUniformLocation(state->shader, "materialType"), 0);
    return viewMat;
}

Vec2d getIsoOrGridPos(GameState* gState, RenderState* rState, Vec2d pos)
{
    return rState->renderIsometric ? getIsoPos(pos, (Vec2i){gState->currentLevel->currentRoom->size.x, gState->currentLevel->currentRoom->size.y}) : pos;
}

void render(GameState* gState, RenderState* state)
{
    // Clear
    {
        Vec4d backgroundCol = state->backgroundColor;
        glClearColor(backgroundCol.x, backgroundCol.y, backgroundCol.z, backgroundCol.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    glViewport(0, 0, state->resolution.x, state->resolution.y);

    glUseProgram(state->shader);

    glUniform1i(glGetUniformLocation(state->shader, "materialType"), 0);
    glUniform1i(glGetUniformLocation(state->shader, "customDepth"), -1);
    Mat3f viewMat;
   
    if (state->renderIsometric)
    {
        if (state->isoMesh2.vertexCount != 0)
        {
            Vec2d offset = Vec2i_to_Vec2d(Vec2i_add(gState->currentLevel->prevRoomCoords, Vec2i_scale(gState->currentLevel->currRoomCoords, -1.0)));
            
            //printf ("offset %f %f %d %d %d %d %d\n", offset.x, offset.y, gState->currentLevel->currentRoom->size.x, gState->currentLevel->currentRoom->size.y,
            //gState->currentLevel->prevRoom->size.x, gState->currentLevel->prevRoom->size.y);
            Vec2i sizeDiff = Vec2i_add(gState->currentLevel->prevRoom->size, Vec2i_scale(gState->currentLevel->currentRoom->size, -1));
            //Vec2i sizeDiff = abs(gState->currentLevel->prevRoom->size.x - gState->currentLevel->currentRoom->size.x) / 2;
            if (offset.x > 0 || offset.y >  0)
            {
                glDisable(GL_DEPTH_TEST);
                renderIsoGrid(gState, state, &state->isoMesh2, (Vec2d){-offset.x * (gState->currentLevel->prevRoom->size.x - 1)  + offset.y * abs(sizeDiff.x / 2), -offset.y * (gState->currentLevel->prevRoom->size.y - 1) + offset.x * abs(sizeDiff.y / 2)});
                glEnable(GL_DEPTH_TEST);
                viewMat = renderIsoGrid(gState, state, &state->isoMesh, (Vec2d){0,0});
            }
            else
            {
                viewMat = renderIsoGrid(gState, state, &state->isoMesh, (Vec2d){0,0});
                glUniform1i(glGetUniformLocation(state->shader, "customDepth"), 0);
                renderIsoGrid(gState, state, &state->isoMesh2, (Vec2d){-offset.x * (gState->currentLevel->currentRoom->size.x - 1) + offset.y * abs(sizeDiff.x / 2), -offset.y * (gState->currentLevel->currentRoom->size.y - 1) + offset.x * abs(sizeDiff.y / 2)});
 glUniform1i(glGetUniformLocation(state->shader, "customDepth"), -1);               
            }
    

        }

      else  viewMat = renderIsoGrid(gState, state, &state->isoMesh, (Vec2d){0,0});

    }
    else
       viewMat = renderGrid(gState, state);

    Entity** entities = gState->currentLevel->currentRoom->entities;

    for(Entity** ent = entities; *ent != NULL; ent++)
    {
        Mat3f viewMatCharacter = Mat3f_multiply(Mat3f_construct(getIsoOrGridPos(gState, state, (*ent)->pos), (Vec2d){1.0f, 1.0f}), viewMat);
        glUniform1i(glGetUniformLocation(state->shader, "flipHorizontal"), Vec2d_rotate((*ent)->velocity, 45.0).x < EPSILON);

        Vec2d modelOffset = (Vec2d){0,0};
        Vec2d shadowOffset;
        float shadowSize;

        getShadowInfo(*ent, &shadowOffset, &modelOffset, &shadowSize);

        Mat3f offsetViewMatCharacter = Mat3f_multiply(Mat3f_construct(modelOffset, (Vec2d){1.0, 1.0}), viewMatCharacter);
        
        {
            Vec2d hitboxSize = Vec2d_add((*ent)->hitbox.topRight, Vec2d_scale((*ent)->hitbox.bottomLeft, -1.0f));
            Vec2d hitboxPos = Vec2d_add((*ent)->hitbox.bottomLeft, Vec2d_scale(hitboxSize, 0.5f));
            Vec2d absoluteHitboxPos = Vec2d_add((*ent)->pos, hitboxPos);
        glUniform1i(glGetUniformLocation(state->shader, "customDepth"),absoluteHitboxPos.x +  absoluteHitboxPos.y + 1 );
        }

        int entityTexID = (*ent)->textureID;
        glUniform1i(glGetUniformLocation(state->shader, "unTexId"), entityTexID);

        glBindTexture(GL_TEXTURE_2D_ARRAY, state->renderIsometric ? state->isoCharacterAtlas : state->characterAtlas);
        glBindVertexArray(state->quadMesh.VAO);

        // outline render
        {
            glDisable(GL_DEPTH_TEST);
            glUniform1i(glGetUniformLocation(state->shader, "materialType"), 4);
            
            float allyColour[4] = {0.1f, 1.0f, 0.2f, 1.0f}, enemyColour[4] = {0.55f, 0.1f, 0.1f, 1.0f}, neutralColour[4] = {0.4f, 0.4f, 0.4f, 1.0f};

            glUniform4fv(glGetUniformLocation(state->shader, "unColor"), 1, isProjectile(*ent) ? neutralColour : ((*ent)->faction == ALLY) ? (allyColour) : (enemyColour));
            glUniformMatrix3fv(glGetUniformLocation(state->shader, "viewMat"), 1, GL_FALSE, offsetViewMatCharacter.d);    

            glDrawArrays(GL_TRIANGLES, 0, 6);
            glEnable(GL_DEPTH_TEST);
        }

        // shadow render
        {
            
            Mat3f shadowMat = Mat3f_multiply(Mat3f_construct(shadowOffset, (Vec2d){1.0, 1.0}), viewMatCharacter);

            glDepthMask(GL_FALSE);

            glUniformMatrix3fv(glGetUniformLocation(state->shader, "viewMat"), 1, GL_FALSE, shadowMat.d);
            glUniform1i(glGetUniformLocation(state->shader, "materialType"), 6);
            glUniform1f(glGetUniformLocation(state->shader, "shadowSize"), shadowSize);

            glDrawArrays(GL_TRIANGLES, 0, 6);

            glDepthMask(GL_TRUE);
        }

        // entity render
        glUniformMatrix3fv(glGetUniformLocation(state->shader, "viewMat"), 1, GL_FALSE, offsetViewMatCharacter.d);
        glUniform1i(glGetUniformLocation(state->shader, "materialType"), 2);
        glDrawArrays(GL_TRIANGLES, 0, 6);


        if (state->bDebugHitboxes)
        {
            Vec2d hitboxSize = Vec2d_add((*ent)->hitbox.topRight, Vec2d_scale((*ent)->hitbox.bottomLeft, -1.0f));
            Vec2d hitboxPos = Vec2d_add((*ent)->hitbox.bottomLeft, Vec2d_scale(hitboxSize, 0.5f));
            Vec2d absoluteHitboxPos = Vec2d_add((*ent)->pos, hitboxPos);
            Mat3f hitboxMat = Mat3f_multiply(Mat3f_construct(getIsoOrGridPos(gState, state, Vec2d_add((*ent)->pos, hitboxPos)), Vec2d_scale(hitboxSize, state->renderIsometric ? 1.0 : 0.5)), viewMat);

            glUniformMatrix3fv(glGetUniformLocation(state->shader, "viewMat"), 1, GL_FALSE, hitboxMat.d);
            glBindTexture(GL_TEXTURE_2D_ARRAY, state->isoTileAtlas);
            glUniform1i(glGetUniformLocation(state->shader, "materialType"), state->renderIsometric ? 2 : 1);
            glUniform1i(glGetUniformLocation(state->shader, "unTexId"), 12);
            glUniform1i(glGetUniformLocation(state->shader, "customDepth"), max(absoluteHitboxPos.x + 0, absoluteHitboxPos.y + 0) + 1);
                 //glUniform1i(glGetUniformLocation(state->shader, "customDepth"), 0);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }


    }

    glUniform1i(glGetUniformLocation(state->shader, "flipHorizontal"), 0);
}

void renderIsometric(GameState*, RenderState*)
{

}