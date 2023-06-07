#include "GLFW/glfw3.h"

void playerMovement(GameState* state, float dt)
{
    Player* player = state->player;
    if (glfwGetKey(state->window, GLFW_KEY_W) == GLFW_PRESS)
    {
        player->entity.velocity += {0.0f, dt};
    }
    if (glfwGetKey(state->window, GLFW_KEY_S) == GLFW_PRESS)
    {
        player->entity.velocity += {0.0f, -dt};
    }
    if (glfwGetKey(state->window, GLFW_KEY_D) == GLFW_PRESS)
    {
        player->entity.velocity += {dt, 0.0f};
    }
    if (glfwGetKey(state->window, GLFW_KEY_A) == GLFW_PRESS)
    {
        player->entity.velocity += {-dt, 0.0f};
    }

    player->entity.velocity = player->entity.SPD * normalize(player->entity.velocity);


}


void move(GameState* state, Entity* entity, float dt)
{
    if (&state->player.entity == entity) // is the player
}

Vec2f* findPath(GameState* state, Entity* entity, Vec2f desiredPosition)
{
    
}