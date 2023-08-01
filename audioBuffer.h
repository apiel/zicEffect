#ifndef _AUDIO_BUFFER_H_
#define _AUDIO_BUFFER_H_

#include "def.h"

// #define AUDIO_BUFFER_SECONDS 30
#define AUDIO_BUFFER_SECONDS 5
#define AUDIO_BUFFER_SIZE SAMPLE_RATE* AUDIO_BUFFER_SECONDS

class AudioBuffer {
public:
    float data[AUDIO_BUFFER_SIZE];
    uint32_t index = 0;

    AudioBuffer()
    {
        for (int i = 0; i < AUDIO_BUFFER_SIZE; i++) {
            data[i] = 0.0;
        }
    }

    void addSample(float sample)
    {
        data[index] = sample;
        index++;
        if (index >= AUDIO_BUFFER_SIZE) {
            index = 0;
        }
    }
};

#endif