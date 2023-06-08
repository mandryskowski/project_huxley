#include "room.h"

TileType getTile(Vec2i vec, GameState *state)
{
    return state->currentRoom->tiles[vec.x][vec.y].type;
}