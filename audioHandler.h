#ifndef _AUDIO_HANDLER_H_
#define _AUDIO_HANDLER_H_

#include "audioBuffer.h"
#include "def.h"
#include "effectDistortion.h"
#include "effectFilter.h"
#include "effectSampleRateReducer.h"

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
    EffectSampleRateReducer sampleRateReducer;

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
            out[i] = filter.sample(in[i]);
            out[i] = distortion.sample(out[i]);
            out[i] = sampleRateReducer.sample(out[i]);
            buffer.addSample(out[i], i);
        }
    }
};

AudioHandler* AudioHandler::instance = NULL;

#endif