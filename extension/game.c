#include "game.h"
//#include "entity.h"
#include "room.h"
#include "glad/glad.h"
#include "glfw/glfw3.h"
#include "render.h"
#include "assets.h"
#include "math.h"

#include <stdio.h>
#include <stdlib.h>


void handleEvents(GameState* state)
{
    //if ()
}
void update(GameState* state, float dt)
{
    /*Entity* entity = state->currentRoom->entities;

    playerMovement(state, dt);
    while (entity != NULL)
    {
        move(state->currentRoom, entity, dt);
    }*/
}

void initGame(GameState* state)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    state->window = glfwCreateWindow(800, 800, "Huxley game", NULL, NULL);

    if (state->window == NULL)
    {
        printf("GLFW error.\n");
        exit(-1);
    }

    glfwMakeContextCurrent(state->window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("GLAD error\n");
        exit(-1);
    }
}

void renderGame(RenderState* rState, GameState* gState)
{
    render(rState);

    glfwSwapBuffers(gState->window);
}
void gameLoop(GameState* gState)
{
    const float timestep = 1.0f / 60.0f;
    float lastUpdateTime = glfwGetTime();
    RenderState rState;
    Room room;
    room.entities = NULL;
    for (int x = 0; x < 16; x++)
    {
        for (int y = 0; y < 16; y++)
        {
            room.tiles[x][y] = (Tile){.textureID = 0, .type = TILE_FLOOR};
        }
    }

    room.tiles[4][12] = (Tile){.textureID = 1, .type = TILE_BARRIER};
    room.tiles[11][4] = (Tile){.textureID = 1, .type = TILE_BARRIER};

    for (int i = 0; i < 16; i++)
    {
        room.tiles[i][0] = (Tile){.textureID = 2, .type = TILE_WALL};
        room.tiles[i][15] = (Tile){.textureID = 2, .type = TILE_WALL};
        room.tiles[0][i] = (Tile){.textureID = 2, .type = TILE_WALL};
        room.tiles[15][i] = (Tile){.textureID = 2, .type = TILE_WALL};
    }

    for (int i = 4; i <= 11; i++)
      room.tiles[i][6] = (Tile){.textureID = 2, .type = TILE_WALL};
    
    room.tiles[7][5] = (Tile){.textureID = 3, .type = TILE_HOLE};
    room.tiles[9][8] = (Tile){.textureID = 3, .type = TILE_HOLE};
    room.tiles[13][4] = (Tile){.textureID = 3, .type = TILE_HOLE};
    gState->currentRoom = &room;


    initRenderState(gState, &rState);
    glEnable(GL_MULTISAMPLE); 

    rState.tileAtlas = loadAtlas("textures.png", 1, 4);
    rState.characterAtlas = loadAtlas("character.png", 1, 1);
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(rState.shader, "atlas"), 0);
    Mat3f viewMat = Mat3f_construct((Vec2f){-0.5f, -0.5f}, (Vec2f){1.0f / 16.0f, 1.0f / 16.0f});
    Mat3f_print(&viewMat);
    glUniformMatrix3fv(glGetUniformLocation(rState.shader, "viewMat"), 1, GL_FALSE, viewMat.d);

    while (!glfwWindowShouldClose(gState->window))
    {
        glfwPollEvents();
        handleEvents(gState);
        float deltaTime = glfwGetTime() - lastUpdateTime;
        if (deltaTime >= timestep)
        {
            update(gState, deltaTime);
            lastUpdateTime = glfwGetTime();
        }

        GLenum err = glGetError();
        if (err)
        {
            printf("GL error %d\n", err);
        }


        renderGame(&rState, gState);
    }
}