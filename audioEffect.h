#ifndef _AUDIO_EFFECT_H_
#define _AUDIO_EFFECT_H_

#include <cstdlib>
using namespace std;

#include "def.h"

#define AUDIO_BUFFER_SECONDS 30
#define AUDIO_BUFFER_SIZE SAMPLE_RATE* AUDIO_BUFFER_SECONDS

class AudioEffect {
protected:
    float buffer[AUDIO_BUFFER_SIZE];

public:
    AudioEffect()
    {
    }

    int64_t samples(float* buf, int len)
    {
        int i = 0;

        for (; i < len; i++) {
            buf[i] = 0;
        }

        // for (uint8_t v = 0; v < MAX_GRAIN_VOICES; v++) {
        //     Voice& voice = voices[v];
        //     if (voice.note != -1) {
        //         for (i = 0; i < len; i++) {
        //             buf[i] += sample(voice);
        //         }
        //     }
        // }

        return i;
    }

    AudioEffect& noteOn(uint8_t note, uint8_t velocity)
    {
        if (velocity == 0) {
            return noteOff(note, velocity);
        }

        // ...

        return *this;
    }

    AudioEffect& noteOff(uint8_t note, uint8_t velocity)
    {
        // ...

        return *this;
    }
};

#endif