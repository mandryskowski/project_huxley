#include "game.h"
#include "entity.h"
#include "room.h"
#include "glad/glad.h"
#include "glfw/glfw3.h"
#include "render.h"
#include "assets.h"
#include "game_math.h"
#include "movement.h"
#include "gui.h"
#include "pathfind.h"

#include <stdio.h>
#include <stdlib.h>

void errorCallback(int lol, const char* str)
{
    printf("Blad %d: %s \n", lol, str);
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

void handleEvents(GameState* state)
{
    updateVelocity(state, GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_D, GLFW_KEY_A, state->player->entity->SPD,
                   &state->player->entity->velocity, state->player->acceleration_const);
    updateVelocity(state, GLFW_KEY_UP, GLFW_KEY_DOWN, GLFW_KEY_RIGHT, GLFW_KEY_LEFT, state->player->entity->attack_SPD,
                   &state->player->entity->attack_velocity, 0);

    Vec2d vel_norm = state->player->entity->velocity;
    Vec2d vel_atk_norm = state->player->entity->attack_velocity;

    if (!Vec2d_zero(vel_atk_norm))
    {
        state->player->entity->attack_velocity = Vec2d_scale(Vec2d_normalize(Vec2d_add(
                Vec2d_scale(vel_norm, state->player->movement_swing),
                Vec2d_scale(vel_atk_norm, 1 - state->player->movement_swing))), state->player->entity->attack_SPD);
    }
}

void update_cooldowns(GameState* state)
{
    for (Entity **entity = state->currentRoom->entities; *entity; entity++)
    {
        (*entity)->cooldown_left = max((*entity)->cooldown_left - 1, 0);
    }
}

void swap(void **this, void **other)
{
    void *helper = *this;
    *this = *other;
    *other = helper;
}

void erase_dead(Room *room)
{
    int entity_cnt = room->entity_cnt;
    for (int i = 1; i < entity_cnt; i++)
    {
        Entity **entity = room->entities + i;
        if (*entity && isDead(*entity))
        {
            free(*entity);
            *entity = NULL;
            swap(entity, (room->entities + room->entity_cnt - 1));
            room->entity_cnt--;
        }
    }
}


void update(GameState* state, double dt)
{
    Entity** arr = state->currentRoom->entities;

    Vec2d* velocities = path((**arr).pos, arr + 1, state);

    int index = 0;
    for(Entity** other = arr+1; *other != NULL; other++)
    {
        if (isProjectile(*other))
        {
            continue;
        }
        (*other)->velocity = Vec2d_scale(*(velocities+index), (*other)->SPD);
        index++;
    }

    if (!Vec2d_zero(state->player->entity->attack_velocity)) {
        handle_attack(state->player->entity, NULL, SPAWN_PROJECTILE);
    }

    move(state, arr, dt);
    update_cooldowns(state);
    erase_dead(state->currentRoom);
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
        const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        rState.resolution = (Vec2i) {mode->width, mode->height};
    }

    Player *player;
    player = Entity_construct_player();

    FILE *file = fopen("predefinedRooms/new_room", "r");
    int height, width;
    fscanf(file, "%d %d", &width, &height);

    Room *room = Room_construct(width, height, file, player);

    gState->currentRoom = room;
    gState->player = player;

    fclose(file);

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
