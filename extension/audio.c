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
        case SOUND_COIN: return "sounds/coin.wav";
        case SOUND_HEALTH: return "sounds/health.wav";
        case SOUND_DEATH_PLAYER: return "sounds/player_death.wav";
        case SOUND_DEATH_WEAK: return "sounds/death_weak.wav";
        case SOUND_DEATH_HEAVY: return "sounds/death_heavy.wav";
        case SOUND_ERROR: return "sounds/error.wav";
        case SOUND_UI_POP: return "sounds/ui_pop.wav";
       // case SOUND_MUSIC_SHOP: return "sounds/music_shop.wav";
        case SOUND_HASKELL_DIES: return "sounds/haskell_death.wav";
        case SOUND_HASKELL_APPEARS: return "sounds/haskell_apperance.wav";
        case MUSIC_MENU: return "sounds/music_menu.wav";
        case MUSIC_SHOP: return "sounds/music_shop.wav";
        case MUSIC_BOSS: return "sounds/music_boss.wav";
        default: printf("Invalid sound type %d \n", type); exit(0);
    }
}

uint findAudioID()
{
    for (int i = 0; i < MAX_SOUND_SOURCES; i++)
    {
        ALint sourceState;
        alGetSourcei(audioState->soundSources[i], AL_SOURCE_STATE, &sourceState);
        if (sourceState != AL_PLAYING)
        {
            return i;
        }
    }
    return MAX_SOUND_SOURCES - 1;
}

void playSound(SoundType type)
{
    alSourcei(playSoundAtPos(type, (Vec2d){0,0}), AL_SOURCE_RELATIVE, AL_TRUE);
}

void playMusic(SoundType begin, SoundType end)
{
    uint id = MAX_SOUND_SOURCES - 1;

    if (begin == NO_SOUND)
    {
        alSourceStop(audioState->soundSources[id]);
        return;
    }
    
    uint buffers[16];
    for (int i = 0; i <= end - begin; i++)
    {
        buffers[i] = audioState->soundBuffers[begin + i];
    }

    uint prevMusicBuffers[16];
    for (int i = 0; i <= audioState->curPlayingMusicEnd - audioState->curPlayingMusicBegin; i++)
    {
        prevMusicBuffers[i] = audioState->soundBuffers[audioState->curPlayingMusicBegin + i];
    }

    alSourcei(audioState->soundSources[id], AL_LOOPING, AL_FALSE);
    alSourceStop(audioState->soundSources[id]);
    uint buffersToUnqueue;
    alGetSourcei(audioState->soundSources[id], AL_BUFFERS_PROCESSED, &buffersToUnqueue);

    alSourceUnqueueBuffers(audioState->soundSources[id], buffersToUnqueue, prevMusicBuffers);
    alSourceQueueBuffers(audioState->soundSources[id], end - begin, buffers); 
    alSourcei(audioState->soundSources[id], AL_SOURCE_RELATIVE, AL_TRUE);  
    alSource3f(audioState->soundSources[id], AL_POSITION, 0, 0, 0);
    alSourcePlay(audioState->soundSources[id]);
    alSourcei(audioState->soundSources[id], AL_LOOPING, AL_TRUE);

    audioState->curPlayingMusicBegin = begin;
    audioState->curPlayingMusicEnd = end;
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
    uint id = findAudioID();

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


    alGenBuffers(MUSIC_LAST + 1, aState->soundBuffers);
    for (SoundType i = 1; i <= SOUND_LAST; i++)
    {
        loadWavSound(getSoundPath(i), i, false);
    }
    for (uint buff = MUSIC_FIRST; buff <= MUSIC_LAST;)
    {
        buff = loadWavSound(getSoundPath(buff), buff, true);
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

uint loadWavSound(char* path, uint buffer, bool music)
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

    size_t s = BLOCK_SIZE;

    printf("File %s \n", path);
    
    while (s == BLOCK_SIZE)
    {
        printf("Sound Block!\n");
        s = tinywav_read_f(&tw, samplePtrs, BLOCK_SIZE);
        
        if (!music && s == BLOCK_SIZE)
        {
            printf("Invalid sound file \n");
            exit(-1);
        }

        char samplesUint[NUM_CHANNELS * BLOCK_SIZE];
        
        for (int i = 0; i < s; i++)
        {
            samplesUint[i] = samples[i] * 128.0f;
        }

    
        alBufferData(audioState->soundBuffers[buffer++], AL_FORMAT_MONO16, samplesUint, s - s % 4, tw.h.SampleRate / 2);
    }


    tinywav_close_read(&tw);
    return buffer;
}