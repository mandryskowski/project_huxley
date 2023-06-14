#include "game.h"
#include "entity.h"
#include "room.h"
#include <math.h>
#include "glad/glad.h"
#include "glfw/glfw3.h"
#include "render.h"
#include "assets.h"
#include "game_math.h"
#include "movement.h"
#include "gui.h"
#include "pathfind.h"
#include "util.h"
#include "level.h"

#include <stdio.h>
#include <stdlib.h>

void errorCallback(int lol, const char* str)
{
    printf("Error %d: %s \n", lol, str);
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    RenderState* rState = glfwGetWindowUserPointer(window);
    rState->resolution = (Vec2i){width, height};
}

void updateVelocity(GameState* state, int up, int down, int right, int left, double max_spd, Vec2d *velocity, double acceleration)
{
    Vec2d velChange = (Vec2d){0.0f, 0.0f};
    if (glfwGetKey(state->window, up) == GLFW_PRESS)
    {
        velChange.y += 1.0f;
    }
    if (glfwGetKey(state->window, down) == GLFW_PRESS)
    {
        velChange.y -= 1.0f;
    }
    if (glfwGetKey(state->window, right) == GLFW_PRESS)
    {
        velChange.x += 1.0f;
    }
    if (glfwGetKey(state->window, left) == GLFW_PRESS)
    {
        velChange.x -= 1.0f;
    }

    velChange = Vec2d_rotate(Vec2d_scale(Vec2d_normalize(velChange), max_spd), -45);
    *velocity = Vec2d_add(Vec2d_scale(*velocity, acceleration), Vec2d_scale(velChange, 1.0f - acceleration));
}

void updateDialogue(GameState* state)
{
    if(state->player->isInDialogue)
    {
        if(state->player->lastSkip + state->guiState->dialogue->skipCooldown < glfwGetTime())
        {
            state->guiState->dialogue->isSkippable = true;

            if(glfwGetKey(state->window, GLFW_KEY_E))
            {
                state->guiState->dialogue->isSkippable = false;
                state->guiState->dialogue->dialogueIndex++;

                if(state->guiState->dialogue->dialogueIndex == state->guiState->dialogue->dialogueSize)
                {
                    state->player->isInDialogue = false;
                    return;
                }

                state->player->lastSkip = glfwGetTime();
            }
        }
    }

    else
    {
        fprintf(stderr, "Entity is not in dialogue\n");
        exit(0);
    }
}

void handleEvents(GameState* state)
{
    if(state->player->isInDialogue)
    {
        updateDialogue(state);
        return;
    }

    updateVelocity(state, GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_D, GLFW_KEY_A, state->player->entity->SPD,
                   &state->player->entity->velocity, state->player->acceleration_const);
    updateVelocity(state, GLFW_KEY_UP, GLFW_KEY_DOWN, GLFW_KEY_RIGHT, GLFW_KEY_LEFT, state->player->entity->attack_SPD,
                   &state->player->entity->attack_velocity, 0);

    if (!Vec2d_zero(state->player->entity->attack_velocity))
    {
        state->player->entity->attack_velocity = Vec2d_scale(Vec2d_normalize(Vec2d_add(
                Vec2d_scale(state->player->entity->velocity , state->player->movement_swing),
                Vec2d_scale(state->player->entity->attack_velocity, 1 - state->player->movement_swing))), state->player->entity->attack_SPD);
    }
}

void update_cooldowns(GameState* state)
{
    for (Entity **entity = state->currentLevel->currentRoom->entities; *entity; entity++)
    {
        (*entity)->cooldown_left = max((*entity)->cooldown_left - 1, 0);
    }
}

void erase_dead(Room *room)
{
    int entity_cnt = room->entity_cnt;
    for (int i = 1; i < entity_cnt; i++)
    {
        Entity **entity = room->entities + i;
        if (*entity && isDead(*entity))
        {
            if ((*entity)->death_func)
            {
                (*entity)->death_func(*entity);
            }
            free(*entity);
            *entity = NULL;
            swap(entity, (room->entities + room->entity_cnt - 1));
            room->entity_cnt--;
        }
    }
}

static int frame_cnt = 0;

void update(GameState* state, double dt)
{
    frame_cnt++;
    state->renderNewRoom = false;
    Entity** arr = state->currentLevel->currentRoom->entities;


    if (frame_cnt % 5 == 0)
    {
        int index = 0;
        Vec2d* velocities = path((**arr).pos, arr + 1, state);
        for(Entity** other = arr+1; *other != NULL; other++)
        {
            if (isProjectile(*other))
            {
                continue;
            }
            (*other)->velocity = Vec2d_scale(*(velocities+index), (*other)->SPD);
            index++;
        }
    }

    if (!Vec2d_zero(state->player->entity->attack_velocity)) {
        handle_attack(state->player->entity, NULL, SPAWN_ENTITY);
    }
    for (Entity **entity = state->currentLevel->currentRoom->entities + 1; *entity; entity++)
    {
        handle_attack(*entity, NULL, SPAWN_ENTITY);
    }

    move(state, arr, dt);
    update_cooldowns(state);
    erase_dead(state->currentLevel->currentRoom);
    if (state->renderNewRoom)
    {
        jump_to_next_room(state);
    }
}

void initGame(GameState* state)
{
    glfwInit();
    glfwSetErrorCallback(errorCallback);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    GLFW_COCOA_RETINA_FRAMEBUFFER;
    state->window = glfwCreateWindow(1920, 1080, "Huxley game", NULL, NULL);

    if (state->window == NULL)
    {
        printf("GLFW error.\n");
        exit(-1);
    }

    GUIState* guiState = calloc(1, sizeof(GUIState));
    state->guiState = guiState;

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

    {
        glfwGetFramebufferSize(gState->window, &rState.resolution.x, &rState.resolution.y);
    }

    glfwSetWindowUserPointer(gState->window, &rState);
    glfwSetWindowSizeCallback(gState->window, framebufferSizeCallback);

    Player *player;
    player = Entity_construct_player();
    gState->player = player;
    gState->player->isInDialogue = true;
    gState->currentLevel = construct_level(player, 6);
    gState->guiState->dialogue = newDialogue();

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
            if (gState->renderNewRoom)
            {
                refreshRoom(gState, &rState);
                //printf("xdd\n");
            }
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
