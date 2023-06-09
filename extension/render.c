#include "render.h"
#include "glad/glad.h"
#include "math.h"
#include "room.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "entity.h"


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
                      "uniform vec4 modelColor;\n" \
                      "void main() \n" \
                      "{ \n" \
                      "vec4 color = vec4(0.0);\n" \
                      "if (materialType == 0) { \n" \
                      " color = texture(atlas, vec3(fsTexCoord, fsTexID));\n" \
                      "} else if (materialType == 1) { \n" \
                      " color = vec4(1.0, 0.5, 0.31, 1.0);\n" \
                      "}\n" \
                      " if (color.a < 0.5) \n" \
                      "{ \n" \
                      " discard;\n" \
                      "} \n" \
                      "outColor = color;"
                      "}";

typedef struct Vertex
{
    Vec2f position;
    Vec2f texCoord;
    int textureID;
} Vertex;

void initGridVertices(GameState* gameState, Vertex* verts, uint width, uint height)
{
    Vec2f offsets[] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f},  // bottom right triangle
                       {0.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}}; // top left triangle
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            for (int i = 0; i < 6; i++)
            {
                *verts = (Vertex){.position = Vec2f_add(offsets[i], (Vec2f){x, y}),
                                  .texCoord = offsets[i],
                                  .textureID = gameState->currentRoom->tiles[x][y].textureID};
                                  //.textureID = y * 16 + x};
                //printf("vert %f %f \n", verts->position.x, verts->position.y);
                verts++;
            }
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
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
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

    // Quad
    {
        Vertex quadVerts[6];
        Vec2f offsets[] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f},  // bottom right triangle
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
}
void render(GameState* gState, RenderState* state)
{
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(state->shader);

    glUniform1i(glGetUniformLocation(state->shader, "materialType"), 0);

    Mat3f viewMat = Mat3f_construct((Vec2f){-0.5f * gState->currentRoom->width / gState->currentRoom->height, -0.5f}, (Vec2f){1.0f / gState->currentRoom->height, 1.0f / gState->currentRoom->height});
    glUniformMatrix3fv(glGetUniformLocation(state->shader, "viewMat"), 1, GL_FALSE, viewMat.d);
    
    glBindTexture(GL_TEXTURE_2D_ARRAY, state->tileAtlas);
    glBindVertexArray(state->LevelVAO);
    glDrawArrays(GL_TRIANGLES, 0, gState->currentRoom->width * gState->currentRoom->height * 6);

    Mat3f viewMatCharacter = Mat3f_multiply(Mat3f_construct(gState->player->entity.pos, (Vec2f){1.0f, 1.0f}), viewMat);
    //Mat3f viewMatCharacter = Mat3f_construct((Vec2f){0.0f, 0.0f}, (Vec2f){1.0f, 1.0f});
    //viewMatCharacter = Mat3f_multiply(viewMatCharacter, Mat3f_construct((Vec2f){0.0f, 0.0f}, (Vec2f){1.0f, 1.0f}));
    //Mat3f_print(&viewMatCharacter);
    glUniformMatrix3fv(glGetUniformLocation(state->shader, "viewMat"), 1, GL_FALSE, viewMatCharacter.d);

    glBindTexture(GL_TEXTURE_2D_ARRAY, state->characterAtlas);
    glBindVertexArray(state->QuadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    if (state->bDebugHitboxes)
    {
        Vec2f hitboxSize = Vec2f_add(gState->player->entity.hitbox.topRight, Vec2f_scale(gState->player->entity.hitbox.bottomLeft, -1.0f));
        Vec2f hitboxPos = Vec2f_add(gState->player->entity.hitbox.bottomLeft, Vec2f_scale(hitboxSize, 0.5f));
        Mat3f hitboxMat = Mat3f_multiply(Mat3f_construct(Vec2f_add(gState->player->entity.pos, hitboxPos), hitboxSize), viewMat);


        Vec2f_print(hitboxPos);
        glUniformMatrix3fv(glGetUniformLocation(state->shader, "viewMat"), 1, GL_FALSE, hitboxMat.d);
        glUniform1i(glGetUniformLocation(state->shader, "materialType"), 1);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}