#ifndef _AUDIO_BUFFER_H_
#define _AUDIO_BUFFER_H_

#include "def.h"

#define AUDIO_BUFFER_SECONDS 5
#define AUDIO_BUFFER_SIZE SAMPLE_RATE* AUDIO_BUFFER_SECONDS

class AudioBuffer {
public:
    // keep in memory current samples chunk
    float input[APP_AUDIO_CHUNK * APP_CHANNELS];
    // keep in memory 5 seconds of samples
    float samples[AUDIO_BUFFER_SIZE];
    uint32_t index = 0;

    AudioBuffer()
    {
        for (int i = 0; i < AUDIO_BUFFER_SIZE; i++) {
            samples[i] = 0.0;
        }
        for (int i = 0; i < APP_AUDIO_CHUNK * APP_CHANNELS; i++) {
            input[i] = 0.0;
        }
    }

    void addSample(float sample, uint16_t i)
    {
        input[i] = sample;
        samples[index] = sample;
        index++;
        if (index >= AUDIO_BUFFER_SIZE) {
            index = 0;
        }
    }
};

#endif