#include "audio.h"
#include "AL/al.h"
#include "AL/alc.h"
#include "tinywav/tinywav.h"

#include <stdio.h>
#include <stdlib.h>

#define NUM_CHANNELS 2
#define BLOCK_SIZE 480

void initAudio(AudioState* aState)
{
    aState->device = alcOpenDevice(NULL);
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

    TinyWav tw;
    tinywav_open_read(&tw, "pop.wav", TW_SPLIT);

    
    for (int i = 0; i < 100; i++) {
    // samples are always provided in float32 format, 
    // regardless of file sample format
    float samples[NUM_CHANNELS * BLOCK_SIZE];
    
    // Split buffer requires pointers to channel buffers
    float* samplePtrs[NUM_CHANNELS];
    for (int j = 0; j < NUM_CHANNELS; ++j) {
        samplePtrs[j] = samples + j*BLOCK_SIZE;
    }

    printf("audio %d \n", tinywav_read_f(&tw, samplePtrs, BLOCK_SIZE));
    
    }

    tinywav_close_read(&tw);


    alGenSources(1, &aState->shootingSound);
   // alSourcei(aState->shootingSound, AL_BUFFER, samplePtrs)
}