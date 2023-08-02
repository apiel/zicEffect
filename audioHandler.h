#ifndef _AUDIO_HANDLER_H_
#define _AUDIO_HANDLER_H_

#include "audioBuffer.h"
#include "def.h"
#include "distortion.h"
#include "filter.h"

class AudioHandler {
protected:
    static AudioHandler* instance;

    // Keep buffer for echo, delay, granular, etc.
    AudioBuffer buffer;

    AudioHandler()
    {
    }

public:
    Filter filter;
    Distortion distortion;

    static AudioHandler& get()
    {
        if (!instance) {
            instance = new AudioHandler();
        }
        return *instance;
    }

    void samples(float* in, float* out, int len)
    {
        for (int i = 0; i < len; i++) {
            out[i] = distortion.sample(filter.sample(in[i]));
            buffer.addSample(out[i], i);
        }
    }
};

AudioHandler* AudioHandler::instance = NULL;

#endif