#ifndef _AUDIO_HANDLER_H_
#define _AUDIO_HANDLER_H_

#include "def.h"
#include "distortion.h"
#include "filter.h"

#define AUDIO_BUFFER_CHUNK_COUNT 10
#define AUDIO_BUFFER_SIZE AUDIO_BUFFER_CHUNK_COUNT * APP_AUDIO_CHUNK

class AudioHandler {
protected:
    float buffer[AUDIO_BUFFER_SIZE];
    uint16_t outputIndex = 0;
    uint16_t inputIndex = 0;

    static AudioHandler* instance;

    AudioHandler()
    {
        for (int i = 0; i < AUDIO_BUFFER_SIZE; i++) {
            buffer[i] = 0;
        }
    }

public:
    Filter filter;
    Distortion distortion;

    uint8_t stepCounter = 0;

    static AudioHandler& get()
    {
        if (!instance) {
            instance = new AudioHandler();
        }
        return *instance;
    }

    void samples(float* buf, int len)
    {
        for (int i = 0; i < len; i++) {
            outputIndex = (outputIndex + 1) % AUDIO_BUFFER_SIZE;
            buffer[outputIndex] = distortion.sample(filter.sample(buffer[outputIndex]));
            buf[i] = buffer[outputIndex];
        }
    }

    void inputSamples(float* buf, int len)
    {
        for (int i = 0; i < len; i++) {
            inputIndex = (inputIndex + 1) % AUDIO_BUFFER_SIZE;
            // printf("%f\n", buf[i]);
            buffer[inputIndex] = buf[i];
        }
    }
};

AudioHandler* AudioHandler::instance = NULL;

#endif