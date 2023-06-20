#include "audio.h"
#include "AL/al.h"
#include "AL/alc.h"
#include "tinywav/tinywav.h"
#include "game_math.h"

#include <stdio.h>
#include <stdlib.h>

#define NUM_CHANNELS 1
#define BLOCK_SIZE 480*1000

static AudioState* audioState;

char* getSoundPath(SoundType type)
{
    switch (type)
    {
        case SOUND_SHOOT: return "sounds/pop.wav";
        case SOUND_EXPLODE: return "sounds/boom.wav";
        default: printf("Invalid sound type %d \n", type); exit(0);
    }
}

void removeSoundSource(uint source)
{
    audioState->usedSoundSources[source] = false;
    printf("removed source %d \n", source);
}

uint addSoundSource(SoundType type)
{
    uint id;
    for (int i = 0; i < MAX_SOUND_SOURCES; i++)
    {
        if (audioState->usedSoundSources[i])
        {
            continue;
        }

        ALint sourceState;
        alGetSourcei(audioState->soundSources[i], AL_SOURCE_STATE, &sourceState);
        if (sourceState != AL_PLAYING)
        {
            id = i;
            break;
        }
    }
    playSoundAtSource(audioState, id, type);
    audioState->usedSoundSources[id] = true;
    return id;
}

void playSoundAtSource(AudioState* aState, uint source, SoundType type)
{
    //printf("sound %d id %d \n", type, source);
    alSourcei(aState->soundSources[source], AL_BUFFER, aState->soundBuffers[type]);   
    alSourcePlay(aState->soundSources[source]);
}

void initAudio(AudioState* aState, char* deviceName, bool firstInit)
{
    if (firstInit)
    {
        for (int i = 0; i < MAX_SOUND_SOURCES; i++)
        {
            aState->usedSoundSources[i] = false;
        }
    }


    audioState = aState;
    aState->device = alcOpenDevice(deviceName);
    if(!aState->device)
        return;
    
    aState->context = alcCreateContext(aState->device, NULL);
    if(aState->context == NULL)
    {
        printf ("Audio error: cannot create audio context.\n");
        return;
    }
    
    if(alcMakeContextCurrent(aState->context) != ALC_TRUE)
    {
        printf ("Audio error: cannot make audio context current.\n");
        return;
    }

    //alSourcei(aState->shootingSound, AL_LOOPING, AL_TRUE);
    alGenSources(MAX_SOUND_SOURCES, aState->soundSources);


    alGenBuffers(SOUND_LAST + 1, aState->soundBuffers);
    for (SoundType i = 0; i <= SOUND_LAST; i++)
    {
        loadWavSound(getSoundPath(i), aState->soundBuffers[i]);
    }
 
    addSoundSource(SOUND_EXPLODE);

}

void cleanupAudio(AudioState* aState)
{
    alDeleteSources(MAX_SOUND_SOURCES, aState->soundSources);
    alDeleteBuffers(SOUND_LAST + 1, aState->soundBuffers);

    alcDestroyContext(aState->context);
    alcCloseDevice(aState->device);
}

void loadWavSound(char* path, uint buffer)
{
    TinyWav tw;
    tinywav_open_read(&tw, path, TW_SPLIT);

    // samples are always provided in float32 format, 
    // regardless of file sample format
    float* samples = malloc(sizeof(float) * NUM_CHANNELS * BLOCK_SIZE);
    
    // Split buffer requires pointers to channel buffers
    float* samplePtrs[NUM_CHANNELS];
    for (int j = 0; j < NUM_CHANNELS; ++j) {
        samplePtrs[j] = samples + j*BLOCK_SIZE;
    }
    size_t s = 0;
    
    for (int i = 0; i < 1; i++) {

    s = tinywav_read_f(&tw, samplePtrs, BLOCK_SIZE);
    
    }

    char samplesUint[NUM_CHANNELS * BLOCK_SIZE];
    //printf("%s sample count: %d \n", path, s);
    for (int i = 0; i < s; i++)
    {
        //printf("sample %f \n", samples[i]);
        samplesUint[i] = samples[i] * 128.0f;
    }

    tinywav_close_read(&tw);
    
    alBufferData(buffer, AL_FORMAT_MONO16, samplesUint, s - s % 4, tw.h.SampleRate / 2);
}