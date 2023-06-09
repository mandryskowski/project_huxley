#ifndef GUI_H
#define GUI_H

typedef struct GameState GameState;
typedef struct RenderState RenderState;

void gui_init(GameState* gState);
void gui_terminate(GameState* gState);
void gui_render();
void gui_update(GameState* state, RenderState* rState);

#endif