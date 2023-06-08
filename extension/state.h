#ifndef STATE_H
#define STATE_H

typedef struct GLFWwindow GLFWwindow;
typedef struct ImGuiContext ImGuiContext;
typedef struct ImGuiIO ImGuiIO;
typedef struct Room Room;
typedef struct Player Player;
typedef struct GameState
{
    struct GLFWwindow* window;
    struct ImGuiContext* ctx;
    struct ImGuiIO* io;
    struct Room* currentRoom;
    struct Player* player;
} GameState;
#endif // STATE_H