#ifndef STATE_H
#define STATE_H

#include <stdbool.h>


typedef struct GLFWwindow GLFWwindow;
typedef struct ImGuiContext ImGuiContext;
typedef struct ImGuiIO ImGuiIO;
//typedef struct Room Room;
typedef struct Level Level;
typedef struct Player Player;
typedef struct Dialogue Dialogue;

typedef struct GUIState
{
    Dialogue* dialogue;
} GUIState;

typedef struct GameState
{
    GLFWwindow* window;
    ImGuiContext* ctx;
    ImGuiIO* io;
    Level* currentLevel;
    Player* player;
    GUIState* guiState;
    bool renderNewRoom;
} GameState;


#endif // STATE_H