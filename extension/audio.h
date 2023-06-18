#ifndef AUDIO_H
#define AUDIO_H

typedef struct ALCdevice ALCdevice;
typedef struct ALCcontext ALCcontext;
typedef unsigned int ALuint;

typedef struct AudioState
{
    ALCdevice* device;
    ALCcontext* context;

    ALuint shootingSound;
} AudioState;

void initAudio(AudioState*);

#endif // AUDIO_H