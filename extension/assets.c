#include "assets.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include "game_math.h"
#include "glad/glad.h"
#include "entity.h"

unsigned char** cutImages(unsigned char* uncutPtr, Vec2i atlasCounts, Vec2i tileSize, int channels)
{
    const int nrImages = atlasCounts.x * atlasCounts.y;
    unsigned char** images = calloc(nrImages, sizeof (unsigned char*));
    for (int i = 0; i < nrImages; i++)
    {
        images[i] = malloc(tileSize.x * tileSize.y * channels);
    }
    for (int y = 0; y < atlasCounts.y; y++)
    {
        for (int line = 0; line < tileSize.y; line++)
        {
            for (int x = 0; x < atlasCounts.x; x++)
            {
                memcpy(images[x + y * atlasCounts.x] + line * tileSize.x * channels,
                        uncutPtr,
                        tileSize.x * channels);
                uncutPtr += tileSize.x * channels;
            }
        }
    }
    return images;
}

uint loadAtlas(char* filename, int width, int height)
{
    stbi_set_flip_vertically_on_load(1);
    int imgWidth, imgHeight, imgChannels;
    unsigned char* data = stbi_load(filename, &imgWidth, &imgHeight, &imgChannels, 0);

    if (data == NULL)
    {
        printf("Could not load atlas texture %s\n", filename);
        return 0;
    }

    int nrTiles = width * height;
    int tileWidth = imgWidth / width;
    int tileHeight = imgHeight / height;

    unsigned char** tiles = cutImages(data, (Vec2i){width, height}, (Vec2i){tileWidth, tileHeight}, imgChannels);

    uint atlasTex;
    glGenTextures(1, &atlasTex);
    glBindTexture(GL_TEXTURE_2D_ARRAY, atlasTex);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, tileWidth, tileWidth, nrTiles, 0, (imgChannels == 4) ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, NULL);

    for (int i = 0; i < nrTiles; i++)
    {
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, tileWidth, tileHeight, 1, (imgChannels == 4) ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, tiles[i]);
        free(tiles[i]);
    }
    // Texture parameters
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    //glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 7);

    stbi_image_free(data);
    free(tiles);

    return atlasTex;
}

uint getMonsterTextureID(MonsterType type)
{
    switch (type)
    {
        case ZOMBIE:
        case SHOOTER:
        case BOMBER:
            return 3;
        case FLYING_SHOOTER:
            return 1;

        case HASKELL:
            return 7;
        case MYSTERIOUS_CHARACTER:
            return 4;
        case MINI_LAMBDA:
            return 9;
        case NOT_MONSTER:
        default:
            return 0;
    }
}