#ifndef _AUDIO_HANDLER_H_
#define _AUDIO_HANDLER_H_

#include "audioBuffer.h"
#include "def.h"
#include "effectDelay.h"
#include "effectDistortion.h"
#include "effectFilter.h"
#include "effectSampleRateReducer.h"

class AudioHandler {
protected:
    static AudioHandler* instance;

    // Keep buffer for echo, delay, granular, etc.
    AudioBuffer buffer;

    AudioHandler()
        : delay(&buffer)
        , delay2(&buffer)
        , delay3(&buffer)
    {
        delay.set(0.4, 0.7, 0.0);
        delay2.set(0.8, 0.5, 0.0);
        delay3.set(1.2, 0.3, 0.0);
    }

public:
    // EffectFilter filter;
    EffectFilter2 filter;
    // EffectFilterMoog filter;
    EffectDistortion distortion;
    EffectSampleRateReducer sampleRateReducer;
    EffectDelay delay;
    EffectDelay delay2;
    EffectDelay delay3;

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

            // out[i] += delay.sample();
            // out[i] += delay2.sample();
            // out[i] += delay3.sample();

            out[i] *= masterVolumeWithGain;
        }
    }
};

AudioHandler* AudioHandler::instance = NULL;

#endif