#ifndef ASSETS_H
#define ASSETS_H

typedef unsigned int uint;
typedef enum MonsterType MonsterType;
uint loadTexture(char* filename);
uint loadAtlas(char* filename, int width, int height);

uint getMonsterTextureID(MonsterType type);

#endif // ASSETS_H