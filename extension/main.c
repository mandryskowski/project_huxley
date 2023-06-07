#include "state.h"

int main()
{
    GameState state;
    initGame(&state);
    gameLoop(&state);
    
    return 0;
}