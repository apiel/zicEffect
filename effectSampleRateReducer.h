#ifndef _EFFECT_SAMPLE_RATE_REDUCER_H_
#define _EFFECT_SAMPLE_RATE_REDUCER_H_

#include "def.h"

#include <math.h>

class EffectSampleRateReducer {
protected:
    float sampleSqueeze;
    int samplePosition = 0;

public:
    uint8_t sampleStep = 10; // the number of samples to double up.

    void samples(float* out, int len)
    {
        if (sampleStep <= 1) {
            return;
        }

        for (int i = 0; i < len;) {
            sampleSqueeze = out[i];
            for (int j = 0; j < sampleStep && i < len; j++) {
                // for each repeated sample, paste in the current
                // root sample, then move onto the next step.
                out[i] = sampleSqueeze;
                i++;
            }
        }
    }

    float sample(float buf)
    {
        if (sampleStep <= 1) {
            return buf;
        }

        if (samplePosition == 0) {
            sampleSqueeze = buf;
        }

        if (samplePosition < sampleStep) {
            samplePosition++;
        } else {
            samplePosition = 0;
        }

        return sampleSqueeze;
    }
};

#endif
