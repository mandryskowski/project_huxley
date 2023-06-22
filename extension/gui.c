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
#include "item.h"
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
    gState->guiState->titleFont = ImFontAtlas_AddFontFromFileTTF(igGetIO()->Fonts, "evil_empire.ttf", 128.0f, ImFontConfig_ImFontConfig(), ImFontAtlas_GetGlyphRangesDefault(igGetIO()->Fonts));
    gState->guiState->defaultFont = ImFontAtlas_AddFontDefault(igGetIO()->Fonts, ImFontConfig_ImFontConfig());
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

void gui_options_window(GameState* gState, RenderState* rState)
{
    if (igSliderFloat("Volume", &gState->aState->volume, 0.0, 1.0, NULL, 0))
    {
        refreshAudioVolume();
        playSound(SOUND_SHOOT);
    }
    igSliderInt2("Resolution", &rState->resolution, 1, 2048, NULL, 0);
    igCheckbox("VSync?", &rState->VSync);

    if (igBeginListBox("Audio device", (ImVec2){0,0}))
    {
        char* s = (char *)alcGetString(NULL, ALC_ALL_DEVICES_SPECIFIER);
        static int current_audio_idx = 0;
        int i = 0;
        while (*s != NULL)
        {
            if (igSelectable_Bool(s, current_audio_idx, 0, (ImVec2){0,0}))
            {
                printf("Selected audio device %s \n", s);
                current_audio_idx = i;

                cleanupAudio(gState->aState);
                initAudio(gState->aState, s);
            }
            i++;
            s += strlen(s) + 1;
        }

        igEndListBox();
    }
}

void guiTextCentered(char* text)
{
    ImVec2 windowSize;
    igGetWindowSize(&windowSize);
    ImVec2 textSize;
    igCalcTextSize(&textSize, text, NULL, false, 0.0);

    igSetCursorPosX((windowSize.x - textSize.x) * 0.5f);
    igText(text);
}

bool guiButtonCentered(char* text, ImVec2 size)
{
    if (size.x == 0 && size.y == 0)
    {
      igCalcTextSize(&size, text, NULL, false, 0.0);
    }
    ImVec2 avail;
    igGetContentRegionAvail(&avail);

    igSetCursorPosX((avail.x - size.x - igGetStyle()->FramePadding.x * 2.0f) * 0.5f);
    return igButton(text, size);
}

void gui_credits_window(GameState* gState, RenderState* rState)
{
    //igPushFont(gState->guiState->titleFont);
    igText("Michal Andryskowski");
    igText("Bogdan Gavra");
    igText("Kuba Lapinski");
    igText("Vlad Marchis");
    igText("Matthew Baugh (Project mentor)");
    igText("The main character's likeness is used with verbal permission of Konstantinos Gkoutzis.");
    igText("EVIL EMPIRE font: license: Creative Commons (by) Attribution, link: https://www.fontspace.com/evil-empire-font-f41587");
    igText("8-bit sounds by LittleRobotSoundFactory https://freesound.org/people/LittleRobotSoundFactory/packs/16681/");
    //igPopFont();
}

void gui_menu_window(GameState* gState, RenderState* rState)
{
    igPushStyleVar_Vec2(ImGuiStyleVar_ItemSpacing, (ImVec2){0.0f, 64.0f});
    igSetWindowFontScale(1.0f / 1.5f);
    guiTextCentered("The DOC's Legacy: Konstantinos' Destiny");
    igSetWindowFontScale(1.0f / 4.0f);

    ImVec2 buttonSize = (ImVec2){igGetIO()->DisplaySize.x * 0.5, igGetIO()->DisplaySize.y * 0.15};

    if (guiButtonCentered("Play", buttonSize))
    {
        gState->guiState->menu = GUI_GAME;
    }
    if (guiButtonCentered("Options", buttonSize))
    {
        gState->guiState->menu = GUI_OPTIONS_MAIN_MENU;
    }
    if (guiButtonCentered("Credits", buttonSize))
    {
        gState->guiState->menu = GUI_CREDITS_MAIN_MENU;
    }
    if (guiButtonCentered("Quit game", buttonSize))
    {
        glfwSetWindowShouldClose(gState->window, true);
    }

    igPopStyleVar(1);
}


void gui_main_menu_update(GameState* gState, RenderState* rState)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    igNewFrame();

    igSetNextWindowPos((ImVec2){0,0}, ImGuiCond_Always, (ImVec2){0,0});
    igSetNextWindowSize(igGetIO()->DisplaySize, ImGuiCond_Always);
    igBegin("Main menu", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
    igSetWindowFontScale(1.0f / 4.0f);

    switch (gState->guiState->menu)
    {
        case GUI_MAIN_MENU: gui_menu_window(gState, rState); break;
        case GUI_OPTIONS_MAIN_MENU:
        {
            gui_options_window(gState, rState);
            if (igButton("Save and exit", (ImVec2){0,0}))
            {
                gState->guiState->menu = GUI_MAIN_MENU;
            }
            break;
        }
        case GUI_OPTIONS_GAME:
        {
            gui_options_window(gState, rState);
            if (igButton("Save and exit", (ImVec2){0,0}))
            {
                gState->guiState->menu = GUI_MAIN_GAME;
            }
            break;
        }

        case GUI_CREDITS_MAIN_MENU:
        {
            gui_credits_window(gState, rState);
            if (igButton("Save and exit", (ImVec2){0,0}))
            {
                gState->guiState->menu = GUI_MAIN_MENU;
            }
            break;
        }
        case GUI_MAIN_GAME:
            if (guiButtonCentered("Options", (ImVec2){igGetIO()->DisplaySize.x * 0.5, igGetIO()->DisplaySize.y * 0.15}))
            {
                gState->guiState->menu = GUI_OPTIONS_GAME;   
            }
            if (guiButtonCentered("Close", (ImVec2){igGetIO()->DisplaySize.x * 0.5, igGetIO()->DisplaySize.y * 0.15}))
            {
                gState->guiState->menu = GUI_GAME;
            }
            if (guiButtonCentered("Return to main menu", (ImVec2){igGetIO()->DisplaySize.x * 0.5, igGetIO()->DisplaySize.y * 0.15}))
            {
                gState->guiState->menu = GUI_MAIN_MENU;
            }
            if (guiButtonCentered("Exit to desktop", (ImVec2){igGetIO()->DisplaySize.x * 0.5, igGetIO()->DisplaySize.y * 0.15}))
            {
                glfwSetWindowShouldClose(gState->window, GLFW_TRUE);
            }
            
            break;
    }
   
    igEnd();
}

void gui_ingame_menu_update(GameState* gState, RenderState* rState)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    igNewFrame(); 
}

void gui_clamp_to_window(GameState* gState)
{
    ImVec2 windowSize, windowPos;
    igGetWindowSize(&windowSize);
    igGetWindowPos(&windowPos);

    windowPos = (ImVec2){max(0, windowPos.x), max(0, windowPos.y)};

    if (windowPos.x + windowSize.x > igGetIO()->DisplaySize.x)
    {
        windowPos.x = igGetIO()->DisplaySize.x - windowSize.x;
    }
    if (windowPos.y + windowSize.y > igGetIO()->DisplaySize.y)
    {
        windowPos.y = igGetIO()->DisplaySize.y - windowSize.y;
    }

    igSetWindowPos_Vec2(windowPos, ImGuiCond_Always);
}

void gui_set_window_pos_to_entity(GameState* gState)
{
    Mat3f viewMat = getViewMatrix(gState, (Vec2d){0,0});
    Vec2d isoPos = getIsoPos(gState->guiState->dialogue->creator->pos);
    Vec3f finalPos = (Vec3f){isoPos.x, isoPos.y, 1.0};
    finalPos = Mat3f_multiply_Vec3f(viewMat, finalPos);
    finalPos.x = finalPos.x * 0.5 + 0.5;
    finalPos.y = 1.0 - (finalPos.y * 0.5 + 0.5);

    igSetWindowPos_Vec2((ImVec2){finalPos.x * igGetIO()->DisplaySize.x, finalPos.y * igGetIO()->DisplaySize.y}, ImGuiCond_Always);
    
    gui_clamp_to_window(gState);
}

void gui_update(GameState* gState, RenderState* rState)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    igNewFrame();


    double minSize = min(igGetIO()->DisplaySize.x, igGetIO()->DisplaySize.y);


    //printf("%d\n", gState->player->canEnterDialogue);
    if(gState->player->canEnterDialogue)
    {
        ImVec2 textSize;
        igCalcTextSize(&textSize, gState->guiState->dialogue->title, NULL, false, 0.0);
        igPushStyleVar_Vec2(ImGuiStyleVar_WindowMinSize, (ImVec2){textSize.x / 6 + igGetStyle()->FramePadding.x * 2, 0});

        igBegin("Dialogue prompt", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);

        igSetWindowFontScale(1.0 / 6.0);
        igText("Press \"Q\" to interact...", NULL, 0);
        gui_set_window_pos_to_entity(gState);

        igEnd();

        igPopStyleVar(1);
    }
    else if(gState->player->isInDialogue)
    {
        ImVec2 textSize;
        igCalcTextSize(&textSize, gState->guiState->dialogue->title, NULL, false, 0.0);
        igPushStyleVar_Vec2(ImGuiStyleVar_WindowMinSize, (ImVec2){textSize.x / 6 + igGetStyle()->FramePadding.x * 2, 0});
        
        igBegin(gState->guiState->dialogue->title, NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);
        igSetWindowFontScale(1.0 / 6.0);
        igText(gState->guiState->dialogue->dialogueLines[gState->guiState->dialogue->dialogueIndex], NULL, 0);
        if(gState->guiState->dialogue->isSkippable)
        {
            if (gState->guiState->dialogue->dialogueIndex + 1 == gState->guiState->dialogue->dialogueSize
                && (gState->currentLevel->currentRoom->type == SHOP_ROOM) && isNPC(gState->guiState->dialogue->creator))
            {
                igText("\nPress \"E\" to reroll items...(otherwise press space)", NULL, 0);
            }
            else if (isItem(gState->guiState->dialogue->creator))
            {
                igText("\nPress \"E\" to pickup item...(otherwise press space)", NULL, 0);
            }
            else
            {
                igText("\nPress \"E\" to continue...", NULL, 0);
            }
        }
        gui_set_window_pos_to_entity(gState);
        igEnd();
        igPopStyleVar(1);
    }

    igPushStyleColor_Vec4(ImGuiCol_WindowBg, (ImVec4){0,0,0,0});
    igPushStyleColor_Vec4(ImGuiCol_Border, (ImVec4){0,0,0,0});

    if (gState->player->entity->HP <= 0)
    {

        igBegin("death window", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        igSetWindowFontScale(1.0);
        igTextColored((ImVec4){1.0, 0.0, 0.0, 0.8 * gState->player->fadeToBlack}, "YOU DIED");
        igSetWindowSize_Vec2((ImVec2){0, 0}, ImGuiCond_Always);


        ImVec2 textSize;
        igCalcTextSize(&textSize, "YOU DIED", NULL, false, 0.0f);
        igSetWindowFontScale(1.0 / 4.0);
        if (gState->player->fadeToBlack == 1.0)
        {
            if (igButton("Return to main menu", (ImVec2){textSize.x,0}))
            {
                gState->guiState->menu = GUI_MAIN_MENU;
            }
        }

        igSetWindowPos_Vec2((ImVec2){igGetIO()->DisplaySize.x / 2 - textSize.x / 2, igGetIO()->DisplaySize.y / 2 - textSize.y}, ImGuiCond_Always);

        igEnd();
    }

    // Items
    if (gState->player->fadeToBlack != 1.0)
    {
        igBegin("Items", NULL, (ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar));
        ImVec2 lol;
        igSetWindowSize_Vec2((ImVec2){0,0}, 0);
        igGetWindowSize(&lol);
        igSetWindowPos_Vec2((ImVec2){igGetIO()->DisplaySize.x - lol.x, igGetIO()->DisplaySize.y - lol.y},  0);


        for (int i = 0; i < gState->player->items_cnt; i++)
        {
            int id = gState->player->items[i]->textureID;
            
            igImage((void*)(intptr_t)gState->rState->uiItemAtlas, (ImVec2){minSize * 0.05, minSize * 0.05}, (ImVec2){(double)(id % 4) * 0.25 + 0.25, (id / 4) * 0.25 + 0.25}, (ImVec2){(double)(id % 4) * 0.25, (id / 4) * 0.25}, (ImVec4){1,1,1,0.5}, (ImVec4){1,1,1,0.5});
            igSameLine(0, minSize * 0.0125);
        }

        if (gState->player->active_item != NULL)
        {
            int id = gState->player->active_item->textureID;
            igImage((void*)(intptr_t)gState->rState->uiItemAtlas, (ImVec2){minSize * 0.05, minSize * 0.05}, (ImVec2){(double)(id % 4) * 0.25 + 0.25, (id / 4) * 0.25 + 0.25}, (ImVec2){(double)(id % 4) * 0.25, (id / 4) * 0.25}, (ImVec4){1,1,1,0.5}, (ImVec4){1,1,0,0.5});
        }
        igEnd();
    }

    // Health and coins
    {
        igBegin("Health and coins", NULL, (ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar));
        ImVec2 lol;
        igSetWindowSize_Vec2((ImVec2){0,0}, 0);
        igGetWindowSize(&lol);
        igSetWindowPos_Vec2((ImVec2){0, igGetIO()->DisplaySize.y - lol.y},  0);
        int id = 10;
        igSetWindowFontScale(1.0 / 4.0);

  igImage((void*)(intptr_t)gState->rState->uiItemAtlas, (ImVec2){32, 32}, (ImVec2){(double)(id % 4) * 0.25 + 0.25, (id / 4) * 0.25 + 0.25}, (ImVec2){(double)(id % 4) * 0.25, (id / 4) * 0.25}, (ImVec4){1,1,1,0.51}, (ImVec4){1,1,1,0.0});
        igSameLine(0, 0);
        igPushStyleColor_Vec4(ImGuiCol_Text, (ImVec4){1,0,0,0.51});
        igText("%d", gState->player->entity->HP);
        igPopStyleColor(1);

id = 6;
        igImage((void*)(intptr_t)gState->rState->uiItemAtlas, (ImVec2){32, 32}, (ImVec2){(double)(id % 4) * 0.25 + 0.25, (id / 4) * 0.25 + 0.25}, (ImVec2){(double)(id % 4) * 0.25, (id / 4) * 0.25}, (ImVec4){1,1,1,0.51}, (ImVec4){1,1,1,0.0});
        igSameLine(0, 0);
        igPushStyleColor_Vec4(ImGuiCol_Text, (ImVec4){1,1,0,0.51});
        igText("%d", gState->player->coins);
        igPopStyleColor(1);
        igEnd();
    }

    igPopStyleColor(2);


    if (gState->guiState->showDebugInfo)
    {
        igSetCurrentFont(gState->guiState->defaultFont);
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
                    igText("Entity cnt: %d", gState->currentLevel->currentRoom->entity_cnt);
                    sliderDoubleN("Hitbox Left Bottom", &(*arr)->hitbox.bottomLeft, 2, -1.0f, 1.0f);
                    sliderDoubleN("Hitbox Top Right", &(*arr)->hitbox.topRight, 2, -1.0f, 1.0f);
                    sliderDoubleN("attack_Velocity", &(*arr)->attack_velocity, 2, 0.0, 5.0);

                    igSetNextItemOpen(true, ImGuiCond_Once);
                    if (igTreeNode_Str("Stats"))
                    {
                        sliderDouble("SPD", &(*arr)->SPD, 0.0, 100.0);
                        igCheckbox("Can fly?", &(*arr)->canFly);
                        igSliderInt("HP", &(*arr)->HP, 0, (*arr)->maxHP, NULL, 0);
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

        igSliderInt("Player coins", &gState->player->coins, 0, 100, NULL, 0);
        igCheckbox("Hitbox Debug", &rState->bDebugHitboxes);
        igCheckbox("Isometric render", &rState->renderIsometric);
        
        igSliderInt2("Resolution", &rState->resolution, 1, 2048, NULL, 0);


        sliderDouble("Player Accel Constant", &gState->player->acceleration_const, 0.0f, 1.0f);
        sliderDouble("Player movement swing", &gState->player->movement_swing, 0.0f, 1.0f);
        igCheckbox("VSync?", &rState->VSync);
        
        igEnd();

        igShowMetricsWindow(NULL);
        igSetCurrentFont(gState->guiState->titleFont);
    }
}
