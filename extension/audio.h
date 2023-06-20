#ifndef AUDIO_H
#define AUDIO_H
#include <stdbool.h>

typedef struct ALCdevice ALCdevice;
typedef struct ALCcontext ALCcontext;
typedef unsigned int uint;
typedef struct Vec2d Vec2d;

#define MAX_SOUND_SOURCES 256

typedef enum
{
    SOUND_SHOOT,
    SOUND_EXPLODE,

    SOUND_LAST = SOUND_EXPLODE
} SoundType;

typedef struct AudioState
{
    ALCdevice* device;
    ALCcontext* context;

    uint soundBuffers[SOUND_LAST + 1];
    uint soundSources[MAX_SOUND_SOURCES];
} AudioState;

void playSound(SoundType type);
uint playSoundAtPos(SoundType, Vec2d position);
void setListenerPos(Vec2d position);

void initAudio(AudioState*, char* deviceName);
void cleanupAudio(AudioState*);

void loadWavSound(char* path, uint buffer);
#endif // AUDIO_H