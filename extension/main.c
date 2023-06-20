#include "game.h"
#include "glfw/glfw3.h"

int main()
{
    GameState state;
    initGame(&state);
    do
    {
        menuLoop(&state);
    } while (!glfwWindowShouldClose(state.window));

    gui_terminate(&state);
    
    return 0;
}