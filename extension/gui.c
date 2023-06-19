#include "gui.h"
#include "glfw/glfw3.h"
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS 1
#define CIMGUI_USE_GLFW 1
#define CIMGUI_USE_OPENGL3
#include "glfw/glfw3.h"
#include "cimgui/cimgui.h"
#include "cimgui/cimgui_impl.h"
#include "state.h"
#include "entity.h"
#include "room.h"
#include "render.h"
#include "game_math.h"
#include "level.h"
#include "AL/alc.h"
#include "audio.h"
#include <string.h>

void gui_init(GameState* gState) {
    // IMGUI_CHECKVERSION();
    gState->ctx = igCreateContext(NULL);
    gState->io  = igGetIO();

    const char* glsl_version = "#version 400 core";
    ImGui_ImplGlfw_InitForOpenGL(gState->window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Setup style
    igStyleColorsDark(NULL);
}

void gui_terminate(GameState* gState) {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    igDestroyContext(gState->ctx);
}

void gui_render() {
    igRender();
    ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
}

bool sliderDouble(char* label, double* ptr, double min, double max)
{
    return igSliderScalar(label, ImGuiDataType_Double, ptr, &min, &max, NULL, 0);
}

bool sliderDoubleN(char* label, double* ptr, uint n, double min, double max)
{
    return igSliderScalarN(label, ImGuiDataType_Double, ptr, n, &min, &max, NULL, 0);
}

void gui_update(GameState* gState, RenderState* rState)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    igNewFrame();

    if (gState->player->entity->HP <= 0)
    {
        igBegin("ur dead loser", NULL, 0);
        igEnd();
    }

    if(gState->player->isInDialogue)
    {
        //igSetNextWindowPos((ImVec2){100, 100}, ImGuiCond_FirstUseEver, (ImVec2){100, 100});
        igBegin(gState->guiState->dialogue->title, NULL, 0);
        igText(gState->guiState->dialogue->dialogueLines[gState->guiState->dialogue->dialogueIndex], NULL, 0);
        if(gState->guiState->dialogue->isSkippable)
        {
           igText("\nPress \"E\" to continue...", NULL, 0);
        }
        igEnd();
    }

    igPushStyleColor_Vec4(ImGuiCol_WindowBg, (ImVec4){0,0,0,0});
    igPushStyleColor_Vec4(ImGuiCol_Border, (ImVec4){0,0,0,0});

    // Items
    {
        igBegin("Items", NULL, (ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar));
        ImVec2 lol;
        igSetWindowSize_Vec2((ImVec2){0,0}, 0);
        igGetWindowSize(&lol);
        igSetWindowPos_Vec2((ImVec2){igGetIO()->DisplaySize.x - lol.x, igGetIO()->DisplaySize.y - lol.y},  0);

        for (int i = 0; i < 16; i++)
        {
            double minSize = min(igGetIO()->DisplaySize.x, igGetIO()->DisplaySize.y);
            
            igImage((void*)(intptr_t)gState->rState->itemAtlas, (ImVec2){minSize * 0.05, minSize * 0.05}, (ImVec2){(double)(i % 4) * 0.25, (i / 4) * 0.25}, (ImVec2){(double)(i % 4) * 0.25 + 0.25, (i / 4) * 0.25 + 0.25}, (ImVec4){1,1,1,0.5}, (ImVec4){1,1,1,0.5});
            igSameLine(0, minSize * 0.0125);
        }
        igEnd();
    }

    // Health
    {
        igBegin("Health", NULL, (ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar));
        ImVec2 lol;
        igSetWindowSize_Vec2((ImVec2){0,0}, 0);
        igGetWindowSize(&lol);
        igSetWindowPos_Vec2((ImVec2){0, igGetIO()->DisplaySize.y - lol.y},  0);
        
        
        //igPushFont();

        igPushStyleColor_Vec4(ImGuiCol_Text, (ImVec4){1,0,0,1});
        igText("%d", gState->player->entity->HP);
        igPopStyleColor(1);

        igEnd();
    }

    igPopStyleColor(2);

    igBegin("Game debug", NULL, 0);

    if (sliderDouble("1:1 Camera size", &gState->player->cameraSize.x, 1.0, 32.0))
    {
        gState->player->cameraSize.y = gState->player->cameraSize.x;
    }
    sliderDoubleN("Camera size", &gState->player->cameraSize, 2, 1.0, 32.0);
    if (igButton("1:1 camera to room size", (ImVec2){0, 0}))
    {
        double size = (double)min(gState->currentLevel->currentRoom->size.x, gState->currentLevel->currentRoom->size.y);
        gState->player->cameraSize = (Vec2d){size, size};
    }
    igSameLine(0, 16.0);
    if (igButton("Stretch camera to room size", (ImVec2){0, 0}))
    {
        gState->player->cameraSize = Vec2i_to_Vec2d(gState->currentLevel->currentRoom->size);
    }
    // IMGUI_DEMO_MARKER("Widgets/Trees/Basic trees");

    if (igButton("Kill all non-player entities", (ImVec2){0, 0}))
    {
        for (int i = 1; i < gState->currentLevel->currentRoom->entity_cnt; i++)
        {
            gState->currentLevel->currentRoom->entities[i]->HP = 0;
        }
    }
    igShowMetricsWindow(NULL);
    if (igTreeNode_Str("Entity tree"))
    {
        Entity** arr = gState->currentLevel->currentRoom->entities;
        int i = 0;
        while (*arr != NULL)
        {
            // Use SetNextItemOpen() so set the default state of a node to be open. We could
            // also use TreeNodeEx() with the ImGuiTreeNodeFlags_DefaultOpen flag to achieve the same thing!
            if (i == 0)
                igSetNextItemOpen(true, ImGuiCond_Once);
                
            if (igTreeNode_Ptr((void*)(intptr_t)i, "Entity %d", i))
            {
                igText("Position %f %f", (*arr)->pos.x, (*arr)->pos.y);
                igText("Velocity %f %f", (*arr)->velocity.x, (*arr)->velocity.y);
                sliderDoubleN("Hitbox Left Bottom", &(*arr)->hitbox.bottomLeft, 2, -1.0f, 1.0f);
                sliderDoubleN("Hitbox Top Right", &(*arr)->hitbox.topRight, 2, -1.0f, 1.0f);
                sliderDoubleN("attack_Velocity", &(*arr)->attack_velocity, 2, 0.0, 5.0);

                igSetNextItemOpen(true, ImGuiCond_Once);
                if (igTreeNode_Str("Stats"))
                {
                    sliderDouble("SPD", &(*arr)->SPD, 0.0, 100.0);
                    igCheckbox("Can fly?", &(*arr)->canFly);
                    igSliderInt("HP", &(*arr)->HP, 0, 100, NULL, 0);
                    igSliderInt("ATK", &(*arr)->ATK, 0, 100, NULL, 0);
                    sliderDouble("ATK speed", &(*arr)->attack_SPD, 0, 100);
                    igSliderInt("ATK cooldown", &(*arr)->attack_cooldown, 0, 100, NULL, 0);
                    igSliderInt("Cooldown left", &(*arr)->cooldown_left, 0, 100, NULL, 0);

                    igTreePop();
                }
                igTreePop();
            }
            i++;
            arr++;
        }
        igTreePop();
    }

    if (igBeginListBox("Audio device", (ImVec2){0,0}))
    {
    char* s = (char *)alcGetString(NULL, ALC_ALL_DEVICES_SPECIFIER);
    static int current_audio_idx = 0;
    int i = 0;
    while (*s != NULL)
    {
     //printf("lista audio: %s \n", s);

     if (igSelectable_Bool(s, current_audio_idx, 0, (ImVec2){0,0}))
     {
        printf("Wybrano %s \n", s);
        current_audio_idx = i;

        cleanupAudio(gState->aState);
        initAudio(gState->aState, s, false);
     }
     i++;
     s += strlen(s) + 1;
    }
    
    

    igEndListBox();
    }

    igCheckbox("Hitbox Debug", &rState->bDebugHitboxes);
    igCheckbox("Isometric render", &rState->renderIsometric);
    
    igSliderInt2("Resolution", &rState->resolution, 1, 2048, NULL, 0);


    sliderDouble("Player Accel Constant", &gState->player->acceleration_const, 0.0f, 1.0f);
    sliderDouble("Player movement swing", &gState->player->movement_swing, 0.0f, 1.0f);
    igCheckbox("VSync?", &rState->VSync);
    
    igEnd();
}
