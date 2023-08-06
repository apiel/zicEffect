#ifndef _AUDIO_HANDLER_H_
#define _AUDIO_HANDLER_H_

#include "audioBuffer.h"
#include "def.h"
#include "effectDistortion.h"
#include "effectFilter.h"
#include "effectBitCrusher.h"

class AudioHandler {
protected:
    static AudioHandler* instance;

    // Keep buffer for echo, delay, granular, etc.
    AudioBuffer buffer;

    AudioHandler()
    {
    }

public:
    EffectFilter filter;
    EffectDistortion distortion;
    EffectBitCrusher bitCrusher;

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

        bitCrusher.samples(out, len);
    }
};

AudioHandler* AudioHandler::instance = NULL;

#endif