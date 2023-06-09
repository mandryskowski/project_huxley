#include "gui.h"
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS 1
#define CIMGUI_USE_GLFW 1
#include "cimgui/cimgui.h"
#include "cimgui/cimgui_impl.h"
#include "state.h"
#include "entity.h"
#include "room.h"
#include "render.h"

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

void gui_update(GameState* gState, RenderState* rState) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    igNewFrame();

    igBegin("Test", NULL, 0);
    igCheckbox("Hitbox Debug", &rState->bDebugHitboxes);
    
    if (igTreeNode_Str("Debug"))
    {
       // IMGUI_DEMO_MARKER("Widgets/Trees/Basic trees");
        if (igTreeNode_Str("Basic trees"))
        {
            Entity** arr = gState->currentRoom->entities;
            int i = 0;
            while (*arr != NULL)
            {
                // Use SetNextItemOpen() so set the default state of a node to be open. We could
                // also use TreeNodeEx() with the ImGuiTreeNodeFlags_DefaultOpen flag to achieve the same thing!

                if (igTreeNode_Ptr((void*)(intptr_t)i, "Entity %d", i))
                {
                    igText("Position %f %f", (*arr)->pos.x, (*arr)->pos.y);
                    igText("Velocity %f %f", (*arr)->velocity.x, (*arr)->velocity.y);
                    if (igSmallButton("button")) {}
                    igTreePop();
                }
                i++;
                arr++;
            }
            igTreePop();
        }
        igTreePop();
    }

    igButton("Test",(struct ImVec2){0,0});
    igEnd();

    // // Normally user code doesn't need/want to call this because positions are saved in .ini file anyway. 
    // // Here we just want to make the demo initial state a bit more friendly!
    // igSetNextWindowPos((struct ImVec2){0,0}, ImGuiCond_FirstUseEver,(struct ImVec2){0,0} ); 
    igShowDemoWindow(NULL);
}