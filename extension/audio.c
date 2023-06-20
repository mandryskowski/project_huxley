#include "audio.h"
#include "AL/al.h"
#include "AL/alc.h"
#include "tinywav/tinywav.h"
#include "game_math.h"
#include "render.h" // for iso pos
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

void playSound(SoundType type)
{
    float pos[3];
    alGetListenerfv(AL_POSITION, pos);
    alSourcei(playSoundAtPos(type, (Vec2d){0,0}), AL_SOURCE_RELATIVE, AL_TRUE);
}

Vec2d getIsoAudioPos(Vec2d mapPos)
{
    Vec2d xOffset = Vec2d_scale((Vec2d){2.0, 1.0}, mapPos.x);
    Vec2d yOffset = Vec2d_scale((Vec2d){-2.0, 1.0}, mapPos.y);
    return Vec2d_add(xOffset, yOffset);
}

uint playSoundAtPos(SoundType type, Vec2d position)
{
    position = getIsoAudioPos(position);
    uint id;
    for (int i = 0; i < MAX_SOUND_SOURCES; i++)
    {
        ALint sourceState;
        alGetSourcei(audioState->soundSources[i], AL_SOURCE_STATE, &sourceState);
        if (sourceState != AL_PLAYING)
        {
            id = i;
            break;
        }
    }
    alSourcei(audioState->soundSources[id], AL_BUFFER, audioState->soundBuffers[type]); 
    alSourcei(audioState->soundSources[id], AL_SOURCE_RELATIVE, AL_FALSE);  
    alSource3f(audioState->soundSources[id], AL_POSITION, position.x, position.y, 0);
    alSourcePlay(audioState->soundSources[id]);

    return audioState->soundSources[id];
}
void setListenerPos(Vec2d position)
{
    position = getIsoAudioPos(position);
    alListener3f(AL_POSITION, position.x, position.y, 0.0);
}

void refreshAudioVolume()
{
    alListenerf(AL_GAIN, audioState->volume);
}

void initAudio(AudioState* aState, char* deviceName)
{


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
 
    refreshAudioVolume();
    playSound(SOUND_SHOOT);

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
    printf("%s sample count: %d \n", path, s);
    for (int i = 0; i < s; i++)
    {
        //printf("sample %f \n", samples[i]);
        samplesUint[i] = samples[i] * 128.0f;
    }

    tinywav_close_read(&tw);
    
    alBufferData(buffer, AL_FORMAT_MONO16, samplesUint, s - s % 4, tw.h.SampleRate / 2);
}