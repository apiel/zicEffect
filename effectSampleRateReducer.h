#ifndef _EFFECT_SAMPLE_RATE_REDUCER_H_
#define _EFFECT_SAMPLE_RATE_REDUCER_H_

#include "def.h"

#include <math.h>

class EffectSampleRateReducer {
protected:
public:
    uint8_t sampleStep = 10; // the number of samples to double up.

    void samples(float* out, int len)
    {
        if (sampleStep <= 1) {
            return;
        }

        for (int i = 0; i < len;) {
            float sampleSqueeze = out[i];
            for (int j = 0; j < sampleStep && i < len; j++) {
                // for each repeated sample, paste in the current
                // root sample, then move onto the next step.
                out[i] = sampleSqueeze;
                i++;
            }
        }
    }
};

#endif
