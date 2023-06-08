#include "render.h"
#include "glad/glad.h"
#include "math.h"
#include "room.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char vShaderSrc[] = "#version 400 core\n" \
                      "layout (location = 0) in vec2 vPosition;\n" \
                      "layout (location = 1) in vec2 vTexCoord;\n" \
                      "layout (location = 2) in uint texID;\n" \
                      "out uint fsTexID;\n" \
                      "out vec2 fsTexCoord;\n" \
                      "void main() \n" \
                      "{ \n" \
                      " gl_Position = vec4(vPosition / vec2(16.0) - vec2(0.5), 0.0, 1.0);\n" \
                      " fsTexID = texID;\n" \
                      " fsTexCoord = vTexCoord;\n" \
                      "}\0";

const char fShaderSrc[] = "#version 400 core\n" \
                      "flat in uint fsTexID;\n" \
                      "in vec2 fsTexCoord;\n" \
                      "out vec4 color;\n" \
                      "uniform sampler2DArray atlas;\n" \
                      "void main() \n" \
                      "{ \n" \
                      "float test = float(fsTexID);\n" \
                      " //color = vec4((test / 256.0) * 0.75 + 0.25, 0.0, 0.0, 1.0);\n" \
                      " color = vec4(texture(atlas, vec3(fsTexCoord, fsTexID)));\n" \
                      "// color = vec4(fsTexCoord, 0.0, 1.0);\n" \
                      "}";

typedef struct Vertex
{
    Vec2f position;
    Vec2f texCoord;
    int textureID;
} Vertex;

void initVertices(GameState* gameState, Vertex* verts, uint width, uint height)
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

void initRenderState(GameState* gState, RenderState* rState)
{
    Vertex vertices[16 * 16 * 6];
    initVertices(gState, vertices, 16, 16);
    glGenBuffers(1, &rState->LevelVBO);
    glGenVertexArrays(1, &rState->LevelVAO);
    glBindBuffer(GL_ARRAY_BUFFER, rState->LevelVBO);
    glBufferData(GL_ARRAY_BUFFER, (sizeof(Vertex)) * 16 * 16 * 6, vertices, GL_STATIC_DRAW);

    glBindVertexArray(rState->LevelVAO);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
    glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, sizeof(Vertex), (void*)offsetof(Vertex, textureID));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
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

void render(RenderState* state)
{
    glUseProgram(state->shader);

    glBindVertexArray(state->LevelVAO);

    glDrawArrays(GL_TRIANGLES, 0, 16 * 16 * 6);
}