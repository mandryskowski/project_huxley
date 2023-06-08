#include "assets.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include "glad/glad.h"

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

    uint atlasTex;
    glGenTextures(1, &atlasTex);
    glBindTexture(GL_TEXTURE_2D_ARRAY, atlasTex);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, tileWidth, tileWidth, nrTiles, 0, (imgChannels == 4) ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, tileWidth, tileHeight, width * height, (imgChannels == 4) ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);

    // Texture parameters
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

    stbi_image_free(data);

    return atlasTex;
}

