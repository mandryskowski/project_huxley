#include "game.h"
#include "entity.h"
#include "room.h"
#include "glad/glad.h"
#include "glfw/glfw3.h"
#include "render.h"
#include "assets.h"
#include "math.h"
#include "movement.h"
#include "gui.h"
#include "pathfind.h"

#include <stdio.h>
#include <stdlib.h>


void handleEvents(GameState* state)
{
    Vec2d velChange = (Vec2d){0.0f, 0.0f};
    if (glfwGetKey(state->window, GLFW_KEY_W) == GLFW_PRESS)
    {
        velChange.y += 1.0f;
    }
    if (glfwGetKey(state->window, GLFW_KEY_S) == GLFW_PRESS)
    {
        velChange.y -= 1.0f;
    }
    if (glfwGetKey(state->window, GLFW_KEY_D) == GLFW_PRESS)
    {
        velChange.x += 1.0f;
    }
    if (glfwGetKey(state->window, GLFW_KEY_A) == GLFW_PRESS)
    {
        velChange.x -= 1.0f;
    }
    
    velChange = Vec2d_scale(velChange, state->player->entity.SPD);
    state->player->entity.velocity = Vec2d_add(Vec2d_scale(state->player->entity.velocity, state->player->acceleration_const),
                                               Vec2d_scale(velChange, 1.0f - state->player->acceleration_const));
}
void update(GameState* state, double dt)
{
    Entity** arr = state->currentRoom->entities;

    Vec2d* velocities = path((**arr).pos, arr + 1, state);

    int index = 0;
    for(Entity** other = arr+1; *other != NULL; other++)
    {
        (*other)->velocity = Vec2d_scale(*(velocities+index), (*other)->SPD);
        index++;
    }

    move(state, arr, dt);

    return;
}

void initGame(GameState* state)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    state->window = glfwCreateWindow(1920, 1080, "Huxley game", glfwGetPrimaryMonitor(), NULL);

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

    gui_init(state);
}

void renderGame(GameState* gState, RenderState* rState)
{
    glfwSwapInterval(rState->VSync);
    render(gState, rState);
    gui_render();
    glfwSwapBuffers(gState->window);
}
void gameLoop(GameState* gState)
{
    const double timestep = 1.0 / 60.0;
    double lastUpdateTime = glfwGetTime();
    RenderState rState = RenderState_construct();
    Room room = Room_construct(24, 16);
    Player player;
    player.entity = Entity_construct();
    player.entity.SPD = 5.0f;
    player.acceleration_const = 0.8;


    gState->currentRoom = &room;
    gState->player = &player;

    Entity enemy = Entity_construct();
    enemy.pos = (Vec2d){10.0f, 10.0f};
    Entity enemy2 = Entity_construct();
    enemy.pos = (Vec2d){9.0f, 9.0f};
    Entity enemy3 = Entity_construct();
    enemy.pos = (Vec2d){8.0f, 8.0f};

    room.entities = calloc(4, sizeof(Entity));
    room.entities[0] = &player.entity;
    room.entities[1] = &enemy;
    room.entities[2] = &enemy2;
    room.entities[3] = &enemy3;


    room.tiles[4][12] = (Tile){.textureID = 1, .type = TILE_BARRIER};
    room.tiles[11][4] = (Tile){.textureID = 1, .type = TILE_BARRIER};

    for (int i = 4; i <= 11; i++)
      room.tiles[i][6] = (Tile){.textureID = 2, .type = TILE_WALL};
    
    room.tiles[7][5] = (Tile){.textureID = 3, .type = TILE_HOLE};
    room.tiles[9][8] = (Tile){.textureID = 3, .type = TILE_HOLE};
    room.tiles[13][4] = (Tile){.textureID = 3, .type = TILE_HOLE};


    initRenderState(gState, &rState);
    glEnable(GL_MULTISAMPLE); 
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(rState.shader, "atlas"), 0);

    while (!glfwWindowShouldClose(gState->window))
    {
        glfwPollEvents();
        double deltaTime = glfwGetTime() - lastUpdateTime;
        if (deltaTime >= timestep)
        {
            handleEvents(gState);
            update(gState, deltaTime);
            lastUpdateTime = glfwGetTime();
        }
        gui_update(gState, &rState);

        GLenum err = glGetError();
        if (err)
        {
            printf("GL error %d\n", err);
        }


        renderGame(gState, &rState);
    }

    gui_terminate(gState);
    glfwTerminate();
}
