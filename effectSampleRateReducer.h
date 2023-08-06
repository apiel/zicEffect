#ifndef _EFFECT_SAMPLE_RATE_REDUCER_H_
#define _EFFECT_SAMPLE_RATE_REDUCER_H_

#include "def.h"

#include <math.h>

class EffectSampleRateReducer {
protected:
    float sampleSqueeze;
    int samplePosition = 0;

    float (EffectSampleRateReducer::*samplePtr)(float) = &EffectSampleRateReducer::skipSample;

    float skipSample(float buf)
    {
        return buf;
    }

    float processSample(float buf)
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

public:
    uint8_t sampleStep = 0; // the number of samples to double up.

    EffectSampleRateReducer()
    {
        set(sampleStep);
    };

    EffectSampleRateReducer set(uint8_t _sampleStep)
    {
        if (_sampleStep == 0) {
            sampleStep = 0;
            samplePtr = &EffectSampleRateReducer::skipSample;
            debug("SampleRateReducer: disabled\n");
            return *this;
        }

        samplePtr = &EffectSampleRateReducer::processSample;
        sampleStep = _sampleStep;

        debug("SampleRateReducer: sampleStep=%d\n", sampleStep);

        return *this;
    }

    float sample(float buf)
    {
        return (this->*samplePtr)(buf);
    }
};

#endif
