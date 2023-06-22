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
    NO_SOUND,
    SOUND_SHOOT,
    SOUND_EXPLODE,
    SOUND_COIN,
    SOUND_HEALTH,

    SOUND_DAMAGE_PLAYER,
    SOUND_DEATH_PLAYER,
    
    SOUND_DEATH_WEAK,
    SOUND_DEATH_HEAVY,

    SOUND_ERROR,
    SOUND_UI_POP,

    SOUND_HASKELL_DIES,
    SOUND_HASKELL_APPEARS,

    SOUND_LAST = SOUND_HASKELL_APPEARS,

    MUSIC_MENU,
    MUSIC_MENU_LAST = MUSIC_MENU + 5,
    MUSIC_SHOP,
    MUSIC_SHOP_LAST = MUSIC_SHOP + 5,
    MUSIC_BOSS,
    MUSIC_BOSS_LAST = MUSIC_BOSS + 2,

    MUSIC_FIRST = MUSIC_MENU,
    MUSIC_LAST = MUSIC_BOSS_LAST
} SoundType;

typedef struct AudioState
{
    ALCdevice* device;
    ALCcontext* context;

    uint soundBuffers[SOUND_LAST + 1];
    uint soundSources[MAX_SOUND_SOURCES];

    float volume;  // between 0.0f and 1.0f

    SoundType curPlayingMusicBegin, curPlayingMusicEnd;
} AudioState;

void playSound(SoundType type);
void playMusic(SoundType begin, SoundType end);
uint playSoundAtPos(SoundType, Vec2d position);
void setListenerPos(Vec2d position);
void refreshAudioVolume();

void initAudio(AudioState*, char* deviceName);
void cleanupAudio(AudioState*);

uint loadWavSound(char* path, uint buffer, bool music);
#endif // AUDIO_H