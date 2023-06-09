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

#include <stdio.h>
#include <stdlib.h>


void handleEvents(GameState* state)
{
    state->player->entity.velocity = (Vec2f){0.0f, 0.0f};
    if (glfwGetKey(state->window, GLFW_KEY_W) == GLFW_PRESS)
    {
        state->player->entity.velocity.y += 1.0f;
    }
    if (glfwGetKey(state->window, GLFW_KEY_S) == GLFW_PRESS)
    {
        state->player->entity.velocity.y -= 1.0f;
    }
    if (glfwGetKey(state->window, GLFW_KEY_D) == GLFW_PRESS)
    {
        state->player->entity.velocity.x += 1.0f;
    }
    if (glfwGetKey(state->window, GLFW_KEY_A) == GLFW_PRESS)
    {
        state->player->entity.velocity.x -= 1.0f;
    }
    
    state->player->entity.velocity = Vec2f_scale(state->player->entity.velocity, state->player->entity.SPD);
}
void update(GameState* state, float dt)
{
    Entity** arr = state->currentRoom->entities;

    Vec2f* velocities = path((**arr).pos, arr + 1, state);

    int index = 0;
    for(Entity** other = arr+1; *other != NULL; other++)
    {
	(*other)->velocity = *(velocities+index);
	index++;
    }

    move(state, arr, dt);

    return;

    //playerMovement(state, dt);
    while (*arr != NULL)
    {
        Entity* entity = *arr;
        printf("entity vel %f %f\n", entity->velocity.x, entity->velocity.y);
                printf("entity pos %f %f\n", entity->pos.x, entity->pos.y);
        //entity->pos = Vec2f_add(entity->pos, Vec2f_scale(entity->velocity, dt));
        arr++;
       // move(state->currentRoom, entity, dt);
    }
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

    gui_init(state);
}

void renderGame(GameState* gState, RenderState* rState)
{
    render(gState, rState);
    gui_render();
    glfwSwapBuffers(gState->window);
}
void gameLoop(GameState* gState)
{
    const float timestep = 1.0f / 60.0f;
    float lastUpdateTime = glfwGetTime();
    RenderState rState;
    Room room = Room_construct(24, 16);
    Player player;
    player.entity = Entity_construct();
    player.entity.SPD = 5.0f;


    gState->currentRoom = &room;
    gState->player = &player;

    Entity enemy = Entity_construct();
    enemy.pos = (Vec2f){10.0f, 10.0f};
    Entity enemy2 = Entity_construct();
    enemy.pos = (Vec2f){9.0f, 9.0f};
    Entity enemy3 = Entity_construct();
    enemy.pos = (Vec2f){8.0f, 8.0f};

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

    rState.tileAtlas = loadAtlas("textures.png", 1, 4);
    rState.characterAtlas = loadAtlas("character.png", 1, 1);
    rState.bDebugHitboxes = false;
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(rState.shader, "atlas"), 0);

    while (!glfwWindowShouldClose(gState->window))
    {
        glfwPollEvents();
        float deltaTime = glfwGetTime() - lastUpdateTime;
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
