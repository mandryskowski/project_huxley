#ifndef STATE_H
#define STATE_H

#include <stdbool.h>

typedef struct GLFWwindow GLFWwindow;
typedef struct ImGuiContext ImGuiContext;
typedef struct ImGuiIO ImGuiIO;
//typedef struct Room Room;
typedef struct Level Level;
typedef struct Player Player;
typedef struct GameState
{
    struct GLFWwindow* window;
    struct ImGuiContext* ctx;
    struct ImGuiIO* io;
    struct Level* currentLevel;
    //struct Room* currentRoom;
    struct Player* player;
    bool renderNewRoom;
} GameState;
#endif // STATE_H