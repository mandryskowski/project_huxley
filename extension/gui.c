#include "gui.h"
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS 1
#define CIMGUI_USE_GLFW 1
#define CIMGUI_USE_OPENGL3
#include "cimgui/cimgui.h"
#include "cimgui/cimgui_impl.h"
#include "state.h"
#include "entity.h"
#include "room.h"
#include "render.h"
#include "math.h"

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

void sliderDouble(char* label, double* ptr, double min, double max)
{
    igSliderScalar(label, ImGuiDataType_Double, ptr, &min, &max, NULL, 0);
}

void sliderDoubleN(char* label, double* ptr, uint n, double min, double max)
{
    igSliderScalarN(label, ImGuiDataType_Double, ptr, n, &min, &max, NULL, 0);
}

void gui_update(GameState* gState, RenderState* rState) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    igNewFrame();

    igBegin("Game debug", NULL, 0);
    igCheckbox("Hitbox Debug", &rState->bDebugHitboxes);
    igCheckbox("Isometric render", &rState->renderIsometric);
    
    igSliderInt2("Resolution", &rState->resolution, 1, 2048, NULL, 0);


    sliderDouble("Player Accel Constant", &gState->player->acceleration_const, 0.0f, 1.0f);
    sliderDouble("Player movement swing", &gState->player->movement_swing, 0.0f, 1.0f);
    // IMGUI_DEMO_MARKER("Widgets/Trees/Basic trees");
    igCheckbox("VSync?", &rState->VSync);
    igShowMetricsWindow(NULL);
    if (igTreeNode_Str("Entity tree"))
    {
        Entity** arr = gState->currentRoom->entities;
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

                if (igTreeNode_Str("Stats"))
                {
                    sliderDouble("SPD", &(*arr)->SPD, 0.0, 100.0);
                    igCheckbox("Can fly?", &(*arr)->canFly);
                    igSliderInt("HP", &(*arr)->HP, 0, 100, NULL, 0);
                    igSliderInt("ATK", &(*arr)->ATK, 0, 100, NULL, 0);

                    igTreePop();
                }
                igTreePop();
            }
            i++;
            arr++;
        }
        igTreePop();
    }

    igEnd();
}
