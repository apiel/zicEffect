#ifndef _EFFECT_SAMPLE_RATE_REDUCER_H_
#define _EFFECT_SAMPLE_RATE_REDUCER_H_

#include "def.h"

#include <math.h>

class EffectSampleRateReducer {
protected:
    float sampleSqueeze;
    int samplePosition = 0;

public:
    uint8_t sampleStep = 0; // the number of samples to double up.

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
