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
#include "animation.h"
#include "audio.h"
#include "item.h"

#include <stdio.h>
#include <stdlib.h>

void errorCallback(int code, const char* str)
{
    printf("Error %d: %s \n", code, str);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    GameState* gState = (GameState*)glfwGetWindowUserPointer(window);
    gState->player->cameraSize.x = clamp(gState->player->cameraSize.x + yoffset * 0.1, 3.0, 32.0);
    gState->player->cameraSize.y = clamp(gState->player->cameraSize.y + yoffset * 0.1, 3.0, 32.0);
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    GameState* gState = glfwGetWindowUserPointer(window);
    gState->rState->resolution = (Vec2i){width, height};
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
        if (glfwGetKey(state->window, GLFW_KEY_SPACE))
        {  
            state->player->isInDialogue = false;
        }

        else if(state->player->lastSkip + state->guiState->dialogue->skipCooldown < glfwGetTime())
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

void updateDialogueState(GameState* state)
{
    if(state->player->canEnterDialogue)
    {
        if(glfwGetKey(state->window, GLFW_KEY_Q))
        {
            state->player->entity->velocity = (Vec2d){0, 0};
            state->player->isInDialogue = true;
            state->guiState->dialogue->dialogueIndex = 0;
            state->player->lastSkip = glfwGetTime();
            state->player->canEnterDialogue = false;
        }
    }

    else
    {
        fprintf(stderr, "Entity cannot enter dialogue\n");
        exit(0);
    }
}

void handleEvents(GameState* state)
{
    if(state->player->canEnterDialogue)
    {
        updateDialogueState(state);
    }

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
        (*entity)->hit_animation = max((*entity)->hit_animation - 1, 0);
    }
    state->player->active_item->cooldown_left = max(state->player->active_item->cooldown_left - 1, 0);
}

void erase_dead(Room *room)
{
    Entity **to_add= calloc(room->entity_cnt,8);
    int to_add_cnt = 0;
    for (int i = 1; i < room->entity_cnt; i++)
    {
        Entity **entity = room->entities + i;
        if (*entity && isDead(*entity) && !(*entity)->hit_animation)
        {
            if ((*entity)->death_func)
            {
                (*entity)->death_func(*entity);
            }
            if (!isProjectile(*entity) && !isMine(*entity) && !isPickable(*entity))
            {
                if (rand() % 5 == 0)
                {
                to_add[to_add_cnt++] = construct_katsu((*entity)->pos, room);
                }
                if (rand() % 7 == 0)
                {
                    to_add[to_add_cnt++] = construct_coin((*entity)->pos, room);
                }
            }

            free(*entity);
            *entity = NULL;
            swap(entity, (room->entities + room->entity_cnt - 1));
            room->entity_cnt--;
        }
    }
    for (Entity **entity = to_add; *entity; entity++)
    {
        printf("%d xdd\n", room->entity_cnt);
        room->entities[room->entity_cnt++] = *entity;
        entity++;
    }
    free(to_add);
}

static int frame_cnt = 0;

void updateLogic(GameState* state, double dt)
{
    frame_cnt++;
    state->renderNewRoom = false;
    Entity** arr = state->currentLevel->currentRoom->entities;


    if (frame_cnt % 5 == 0 && !isClear(state->currentLevel->currentRoom))
    {
        int index = 0;
        Vec2d* velocities = path((**arr).pos, arr + 1, state);
        for(Entity** other = arr+1; *other != NULL; other++)
        {
            if (isProjectile(*other))
            {
                index++;
                continue;
            }

            (*other)->velocity = *(velocities+index);
            index++;
        }
    }

    if (state->player->entity->cooldown_left == 0 && !Vec2d_zero(state->player->entity->attack_velocity))
    {
        playSound(SOUND_SHOOT);
    }

    handle_attack(state->player->entity, NULL, SPAWN_ENTITY);

    if (glfwGetKey(state->window, GLFW_KEY_F) && !state->player->active_item->cooldown_left &&
            !isEmpty(state->player->prev_positions))
    {
        state->player->active_item->item_active(state->player);
        state->player->active_item->cooldown_left = state->player->active_item->active_cooldown;
    }

    for (Entity **entity = state->currentLevel->currentRoom->entities + 1; *entity; entity++)
    {
        if (!Vec2d_zero((*entity)->attack_velocity))
            handle_attack(*entity, state->player->entity, SPAWN_ENTITY);
    }

    move(state, arr, dt);
    update_cooldowns(state);
    erase_dead(state->currentLevel->currentRoom);
    if (state->renderNewRoom)
    {
        jump_to_next_room(state);
    }
}

void updateAnimations(GameState* state, Entity** entities, size_t entity_cnt)
{
    for (int i = 0; i < entity_cnt; i++)
    {
        if (entities[i]->currentAnimation != NULL)
        {
            entities[i]->currentAnimation->animFunc(entities[i]->currentAnimation);
        }
    }

    if (state->player->screenShakeFramesLeft > 0)
        state->player->screenShakeFramesLeft--;
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
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GL_FALSE);
    state->window = glfwCreateWindow(1512, 1037, "Huxley game", NULL, NULL);

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

    state->rState = malloc(sizeof(RenderState));
    *state->rState = RenderState_construct();

    glfwSetWindowUserPointer(state->window, state);
    glfwGetWindowSize(state->window, &state->rState->resolution.x, &state->rState->resolution.y);
    glfwSetFramebufferSizeCallback(state->window, framebufferSizeCallback);
    glfwSetScrollCallback(state->window, scrollCallback);
}

void renderGame(GameState* gState, RenderState* rState)
{
   glfwSwapInterval(0);
    render(gState, rState);
    gui_render();
    glfwSwapBuffers(gState->window);
}
void gameLoop(GameState* gState)
{
    const double timestep = 1.0 / 60.0;
    double lastUpdateTime = glfwGetTime();
    double timeAccumulator = 0.0;

    AudioState aState;
    initAudio(&aState, NULL);
    gState->aState = &aState;


    Player *player;
    player = Entity_construct_player();
    gState->player = player;
    gState->player->isInDialogue = false;
    gState->player->canEnterDialogue = false;
    gState->currentLevel = construct_level(player, 6);
    gState->guiState->dialogue = NULL;

    initRenderState(gState, gState->rState);

    for (Entity **entity = gState->currentLevel->currentRoom->entities + 1; *entity; entity++)
    {
        (*entity)->cooldown_left = 180;
    }

    glfwSwapInterval(gState->rState->VSync);

    while (!glfwWindowShouldClose(gState->window))
    {
        glfwPollEvents();

        double deltaTime = glfwGetTime() - lastUpdateTime;
        timeAccumulator += deltaTime;
        lastUpdateTime = glfwGetTime();
        while (timeAccumulator >= timestep)
        {
            handleEvents(gState);
            updateLogic(gState, timestep);
            updateAnimations(gState, gState->currentLevel->currentRoom->entities, gState->currentLevel->currentRoom->entity_cnt);
            setListenerPos(gState->player->entity->pos);
            if (gState->renderNewRoom)
            {
                refreshRoom(gState, gState->rState);
                //printf("xdd\n");
            }

            timeAccumulator -= timestep;
        }
        gui_update(gState, gState->rState);

        GLenum err = glGetError();
        if (err)
        {
            printf("GL error %d\n", err);
        }

        renderGame(gState, gState->rState);
    }

    gui_terminate(gState);
    glfwTerminate();
}
