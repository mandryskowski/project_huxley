#include "assets.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include "game_math.h"
#include "glad/glad.h"
#include "entity.h"

uint loadTexture(char* filename)
{
    stbi_set_flip_vertically_on_load(1);
    int imgWidth, imgHeight, imgChannels;
    unsigned char* data = stbi_load(filename, &imgWidth, &imgHeight, &imgChannels, 0);

    if (data == NULL)
    {
        printf("Could not load texture %s\n", filename);
        return 0;
    }

    uint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, imgWidth, imgHeight, 0, (imgChannels == 4) ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);

    // Texture parameters
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

    stbi_image_free(data);

    return tex;
}

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
	    return 3;
        case BOMBER:
            return 12;
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

Vec4d grayValueToColor(double val, double alpha)
{
    return (Vec4d){val, val, val, alpha};
}

Vec4d getRoomMinimapColor(RoomType type, bool visited, Vec2i roomCoords)
{
    double checkerboardColor = ((roomCoords.x + roomCoords.y) % 2);
    switch (type)
    {
        case NORMAL_ROOM: return visited ? grayValueToColor(checkerboardColor * 0.15 + 0.75, 0.51) : grayValueToColor(checkerboardColor * 0.1 + 0.3, 0.51);
        case BOSS_ROOM: return visited ? grayValueToColor(checkerboardColor * 0.15 + 0.75, 0.51) : (Vec4d){0.5, 0.0, 0.0, 0.51};
        case ITEM_ROOM: case SHOP_ROOM: return visited ? (Vec4d){0.5, 1.0, 0.0, 0.51} : (Vec4d){0.25, 0.5, 0.0, 0.51};
        case QUEST_ROOM: return visited ? (Vec4d){1.0, 1.0, 0.0, 0.51} : (Vec4d){0.5, 0.5, 0.0, 0.51};
        default:
        case NOT_ROOM: return (Vec4d){0, 0, 0, 0.51};

    }
}


/*EntityTextureInformation getEntityTexInfo(Entity* ent)
{
    if (isItem(ent))
    {
        return (EntityTextureInformation){.shadowOffset = (Vec2d){0.0f, -0.2f}, .shadowStrength = 4.0, .textureOffset = (Vec2d){0,0}, .textureScale = (Vec2d){0.8, 0.8}};
    }
    else if (isPickable(ent))
    {
        return (EntityTextureInformation){.shadowOffset = (Vec2d){0.0f, -0.2f}, .shadowStrength = 16.0, .textureOffset = (Vec2d){0,0}, .textureScale = (Vec2d){0.4, 0.4}};
    }


} */
