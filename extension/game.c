#include <glad/glad.h>
#include <glfw/glfw3.h>

void update(GameState* state, float dt)
{
    Entity* entity = state->currentRoom->entities;

    playerMovement(state, dt);
    while (entity != NULL)
    {
        move(state->currentRoom, entity, dt);
    }
}

void initGame(GameState* state)
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    state->window = glfwCreateWindow(800, 800, "The Binding of Konstantinos", NULL, NULL);

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

void render(GameState* state)
{
    glfwSwapBuffers(state->window);
}
void gameLoop(GameState* state)
{
    const float timestep = 1.0f / 60.0f;
    float lastUpdateTime = glfwGetTime();
    while (true)
    {
        glfwPollEvents();
        float deltaTime = glfwGetTime() - lastUpdateTime;
        if (deltaTime >= timestep)
        {
            update(state, deltaTime);
            lastUpdateTime = glfwGetTime();
        }


        render(state);
    }
}