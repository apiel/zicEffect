#ifndef _AUDIO_BUFFER_H_
#define _AUDIO_BUFFER_H_

#include "def.h"

#ifndef AUDIO_BUFFER_SECONDS
#define AUDIO_BUFFER_SECONDS 5
#endif

// #define AUDIO_BUFFER_SIZE SAMPLE_RATE* AUDIO_BUFFER_SECONDS

template <uint8_t SECONDS = AUDIO_BUFFER_SECONDS>
class AudioBuffer {
public:
    const uint64_t size = SAMPLE_RATE * SECONDS;

    // keep in memory 5 seconds of samples
    float samples[SAMPLE_RATE * SECONDS];
    uint32_t index = 0;

    AudioBuffer()
    {
        for (uint64_t i = 0; i < size; i++) {
            samples[i] = 0.0;
        }
    }

    void addSample(float sample)
    {
        samples[index] = sample;
        index++;
        if (index >= size) {
            index = 0;
        }
    }
};

#endif