#include "render.h"
#include "glad/glad.h"
#include "game_math.h"
#include "room.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "entity.h"
#include "assets.h"

RenderState RenderState_construct()
{
    return (RenderState){.bDebugHitboxes = false, .VSync = true, .renderIsometric = true,
                         .characterAtlas = loadAtlas("character.png", 1, 1), .LevelVAO = 0, .IsoLevelVAO = 0, .QuadVAO = 0, .shader = 0, .tileAtlas = loadAtlas("textures.png", 1, 4),
                         .isoTileAtlas = loadAtlas("isotextures_actual.png", 1, 8), .isoCharacterAtlas = loadAtlas("isocharacter.png", 1, 1),
                         .resolution = (Vec2i){1024, 1024}};
}


const char vShaderSrc[] = "#version 400 core\n" \
                      "layout (location = 0) in vec2 vPosition;\n" \
                      "layout (location = 1) in vec2 vTexCoord;\n" \
                      "layout (location = 2) in uint vTexID;\n" \
                      "flat out uint fsTexID;\n" \
                      "out vec2 fsTexCoord;\n" \
                      "uniform mat3 viewMat;\n" \
                      "void main() \n" \
                      "{ \n" \
                      " gl_Position = vec4(vec2(viewMat * vec3(vPosition, 1.0)), 0.0, 1.0);\n" \
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
                      "uniform vec4 modelColor;\n" \
                      "void main() \n" \
                      "{ \n" \
                      "vec4 color = vec4(0.0);\n" \
                      "if (materialType == 0) { \n" \
                      " color = texture(atlas, vec3(fsTexCoord, fsTexID));\n" \
                      "} else if (materialType == 1) { \n" \
                      " color = vec4(1.0, 0.5, 0.31, 1.0);\n" \
                      "} else if (materialType == 2) {\n" \
                      " color = texture(atlas, vec3(fsTexCoord, unTexId)); \n" \
                      " }if (color.a < 0.5) \n" \
                      "{\n" \
                      " discard;\n" \
                      "} \n" \
                      "outColor = color;\n" \
                      "//outColor = vec4(fsTexCoord, 0.0, 1.0);\n"\
                      "}";

typedef struct Vertex
{
    Vec2d position;
    Vec2d texCoord;
    int textureID;
} Vertex;

void initGridVertices(GameState* gameState, Vertex* verts, uint width, uint height)
{
    Vec2d offsets[] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f},  // bottom right triangle
                       {0.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}}; // top left triangle
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            for (int i = 0; i < 6; i++)
            {
                *verts = (Vertex){.position = Vec2d_add(offsets[i], (Vec2d){x, y}),
                                  .texCoord = offsets[i],
                                  .textureID = gameState->currentRoom->tiles[x][y].textureID};
                                  //.textureID = y * 16 + x};
                //printf("vert %f %f \n", verts->position.x, verts->position.y);
                verts++;
            }
        }
    }
}

Vec2d getIsoPos(Vec2d mapPos, Vec2i roomSize)
{
        float mulx = 0.75, muly = 0.4;
    //width = 2;
   // height = 2;
    Vec2d tileSize = (Vec2d){1.0, 1.0};
    Vec2d xOffset = (Vec2d){tileSize.x * -mulx, tileSize.y * -muly};
    Vec2d yOffset = (Vec2d){tileSize.x * mulx , -tileSize.y * muly};

    return Vec2d_add(Vec2d_scale(xOffset, roomSize.x - 0.5 - mapPos.x), 
                     Vec2d_scale(yOffset, roomSize.y - 0.5 - mapPos.y));
}

void initIsoVertices(GameState* gameState, Vertex* verts, uint width, uint height)
{
    Vec2d offsets[] = {{-1.0, -1.0}, {1.0, -1.0}, {1.0, 1.0},  // bottom right triangle
                       {-1.0, -1.0}, {1.0, 1.0}, {-1.0, 1.0}}; // top left triangle
    float mulx = 0.75, muly = 0.4;
    //width = 2;
   // height = 2;
    Vec2d tileSize = (Vec2d){1.0, 1.0};
    Vec2d xOffset = (Vec2d){tileSize.x * -mulx, tileSize.y * -muly};
    Vec2d yOffset = (Vec2d){tileSize.x * mulx , -tileSize.y * muly};
    for (int x = width - 1; x >= 0; x--)
    {
        for (int y = height - 1; y >= 0; y--)
        {
            const Vec2d quadPos = Vec2d_add(Vec2d_scale(xOffset, (width - 1 - x)), 
                                            Vec2d_scale(yOffset, (height - 1 - y)));
            int texID = 7;
            if (gameState->currentRoom->tiles[x][y].type == TILE_WALL)
            {
                bool rightWall = (y > 0 && gameState->currentRoom->tiles[x][y - 1].type != TILE_WALL);
                bool leftWall = (x > 0 && gameState->currentRoom->tiles[x - 1][y].type != TILE_WALL);
                if (leftWall && rightWall)
                {
                    texID = 5;
                }
                else if (leftWall)
                {
                    texID = 6;
                }
                else if (rightWall)
                {
                    texID = 5;
                }
                else
                {
                    texID = 0;
                }
            }
            else if (gameState->currentRoom->tiles[x][y].type == TILE_BARRIER)
            {
                texID = 2;
            }
            else if (gameState->currentRoom->tiles[x][y].type == TILE_HOLE)
            {
                texID = 3;
            }
            for (int i = 0; i < 6; i++)
            {


                *verts = (Vertex){.position = Vec2d_add((Vec2d){offsets[i].x * tileSize.x, offsets[i].y * tileSize.y}, quadPos),
                                  .texCoord = Vec2d_add((Vec2d){0.5, 0.5}, Vec2d_scale(offsets[i], 0.5)),
                                  .textureID = texID};

                                  //.textureID = gameState->currentRoom->tiles[x][y].textureID};
                                  //.textureID = y * 16 + x};
                //printf("tcoord %f %f\n", verts->texCoord.x, verts->texCoord.y);
                verts++;
            }
           // printf("vert (%d %d) %f %f \n", x, y,quadPos.x, quadPos.y);
        }
    }
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

uint initVAO(GLsizei size, void* data)
{
    uint VAO, VBO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

    glBindVertexArray(VAO);
    glVertexAttribPointer(0, 2, GL_DOUBLE, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glVertexAttribPointer(1, 2, GL_DOUBLE, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
    glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, sizeof(Vertex), (void*)offsetof(Vertex, textureID));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    return VAO;
}

void initRenderState(GameState* gState, RenderState* rState)
{
    // Grid
    {
        Vertex gridVerts[gState->currentRoom->width * gState->currentRoom->height * 6];
        initGridVertices(gState, gridVerts, gState->currentRoom->width, gState->currentRoom->height);

        rState->LevelVAO = initVAO(sizeof(Vertex) * gState->currentRoom->width * gState->currentRoom->height * 6, gridVerts);
    }

    //  ISO Grid
    {
        Vertex gridVerts[gState->currentRoom->width * gState->currentRoom->height * 6];
        initIsoVertices(gState, gridVerts, gState->currentRoom->width, gState->currentRoom->height);

        rState->IsoLevelVAO = initVAO(sizeof(Vertex) * gState->currentRoom->width * gState->currentRoom->height * 6, gridVerts);
    }

    // Quad
    {
        Vertex quadVerts[6];
        Vec2d offsets[] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f},  // bottom right triangle
                       {0.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}}; // top left triangle
        for (int i = 0; i < 6; i++)
        {
            quadVerts[i] = (Vertex){{offsets[i].x * 2.0 - 1.0, offsets[i].y * 2.0 - 1.0},
                            offsets[i],
                            0};
        }

        rState->QuadVAO = initVAO(sizeof(Vertex) * 6, quadVerts);
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
}

void renderGrid(GameState* gState, RenderState* state, Mat3f* viewMat)
{
    *viewMat = Mat3f_construct((Vec2d){-0.5f * gState->currentRoom->width / gState->currentRoom->height, -0.5f}, (Vec2d){1.0f / gState->currentRoom->height, 1.0f / gState->currentRoom->height});
     glUniformMatrix3fv(glGetUniformLocation(state->shader, "viewMat"), 1, GL_FALSE, viewMat->d);
    
    glBindTexture(GL_TEXTURE_2D_ARRAY, state->tileAtlas);
    glBindVertexArray(state->LevelVAO);
    glDrawArrays(GL_TRIANGLES, 0, gState->currentRoom->width * gState->currentRoom->height * 6);
}

void renderIsoGrid(GameState* gState, RenderState* state, Mat3f* viewMat)
{
    *viewMat = Mat3f_construct((Vec2d){0.0 * gState->currentRoom->width / gState->currentRoom->height, 0.1f}, (Vec2d){1.0f / gState->currentRoom->height, 1.0f / gState->currentRoom->height});
    glUniformMatrix3fv(glGetUniformLocation(state->shader, "viewMat"), 1, GL_FALSE, viewMat->d);
    glUniform1i(glGetUniformLocation(state->shader, "materialType"), 0);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  
    glBindTexture(GL_TEXTURE_2D_ARRAY, state->isoTileAtlas);
    glBindVertexArray(state->IsoLevelVAO);
    glDrawArrays(GL_TRIANGLES, 0, gState->currentRoom->width * gState->currentRoom->height * 6);
    //glDrawArrays(GL_TRIANGLES, 0, 4* 6);
}

Vec2d getIsoOrGridPos(GameState* gState, RenderState* rState, Vec2d pos)
{
    return rState->renderIsometric ? getIsoPos(pos, (Vec2i){gState->currentRoom->width, gState->currentRoom->height}) : pos;
}

void render(GameState* gState, RenderState* state)
{
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glViewport(0, 0, state->resolution.x, state->resolution.y);

    glUseProgram(state->shader);

    glUniform1i(glGetUniformLocation(state->shader, "materialType"), 0);

    Mat3f viewMat;
   
    state->renderIsometric ? renderIsoGrid(gState, state, &viewMat) : renderGrid(gState, state, &viewMat);

    Entity** entities = gState->currentRoom->entities;

    for(Entity** ent = entities; *ent != NULL; ent++)
    {
        Mat3f viewMatCharacter = Mat3f_multiply(Mat3f_construct(getIsoOrGridPos(gState, state, (*ent)->pos), (Vec2d){1.0f, 1.0f}), viewMat);
        glUniform1i(glGetUniformLocation(state->shader, "materialType"), 0);
        glUniformMatrix3fv(glGetUniformLocation(state->shader, "viewMat"), 1, GL_FALSE, viewMatCharacter.d);

        glBindTexture(GL_TEXTURE_2D_ARRAY, state->renderIsometric ? state->isoCharacterAtlas : state->characterAtlas);
        glBindVertexArray(state->QuadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        if (state->bDebugHitboxes)
        {
            Vec2d hitboxSize = Vec2d_add((*ent)->hitbox.topRight, Vec2d_scale((*ent)->hitbox.bottomLeft, -1.0f));
            Vec2d hitboxPos = Vec2d_add((*ent)->hitbox.bottomLeft, Vec2d_scale(hitboxSize, 0.5f));
            Mat3f hitboxMat = Mat3f_multiply(Mat3f_construct(getIsoOrGridPos(gState, state, Vec2d_add((*ent)->pos, hitboxPos)), Vec2d_scale(hitboxSize, state->renderIsometric ? 1.0 : 0.5)), viewMat);

            glUniformMatrix3fv(glGetUniformLocation(state->shader, "viewMat"), 1, GL_FALSE, hitboxMat.d);
            glBindTexture(GL_TEXTURE_2D_ARRAY, state->isoTileAtlas);
            glUniform1i(glGetUniformLocation(state->shader, "materialType"), state->renderIsometric ? 2 : 1);
            glUniform1i(glGetUniformLocation(state->shader, "unTexId"), 7);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    }
}

void renderIsometric(GameState*, RenderState*)
{

}