#ifndef _AUDIO_HANDLER_H_
#define _AUDIO_HANDLER_H_

#include "def.h"
#include "distortion.h"
#include "filter.h"
#include "audioBuffer.h"
#include "granular.h"

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
    Granular granular;

    uint8_t stepCounter = 0;

    static AudioHandler& get()
    {
        if (!instance) {
            instance = new AudioHandler();
        }
        return *instance;
    }

    void samples(float* in, float * out, int len)
    {
        for (int i = 0; i < len; i++) {
            out[i] = distortion.sample(filter.sample(in[i]));
            buffer.addSample(out[i]);
        }
        granular.samples(buffer, out, len);
    }
};

AudioHandler* AudioHandler::instance = NULL;

#endif