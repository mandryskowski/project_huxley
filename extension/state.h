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
typedef struct RenderState RenderState;
typedef struct AudioState AudioState; 

typedef enum GUIMenu
{
    GUI_INTRO,
    GUI_MAIN_MENU,
    GUI_OPTIONS_MAIN_MENU,
    GUI_CREDITS_MAIN_MENU,
    GUI_GAME,
    GUI_MAIN_GAME,
    GUI_OPTIONS_GAME
} GUIMenu;

typedef struct GUIState
{
    Dialogue* dialogue;
    GUIMenu menu;
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
    RenderState* rState;
    AudioState* aState;
} GameState;


#endif // STATE_H