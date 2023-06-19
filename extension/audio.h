#ifndef AUDIO_H
#define AUDIO_H
#include <stdbool.h>

typedef struct ALCdevice ALCdevice;
typedef struct ALCcontext ALCcontext;
typedef unsigned int uint;

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
    bool usedSoundSources[MAX_SOUND_SOURCES];
} AudioState;

uint addSoundSource(SoundType type);
void removeSoundSource(uint);
void playSoundAtSource(AudioState*, uint source, SoundType type);

void initAudio(AudioState*, char* deviceName, bool firstInit);
void cleanupAudio(AudioState*);

void loadWavSound(char* path, uint buffer);
#endif // AUDIO_H