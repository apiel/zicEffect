#ifndef _EFFECT_BITCRUSHER_H_
#define _EFFECT_BITCRUSHER_H_

#include "def.h"

#include <math.h>

class EffectBitCrusher {
protected:
public:
    uint8_t crushBits = 6; // 16 = off
    uint8_t sampleStep = 10; // the number of samples to double up.

    void samples(float* out, int len)
    {
        if (crushBits == 16 && sampleStep <= 1) {
            return;
        }

        if (sampleStep <= 1) { // no sample rate mods, just crush the bitdepth.
            for (int i = 0; i < len; i++) {
                int16_t out16 = out[i] * 32767;
                int16_t sampleSquidge = out16 >> (16 - crushBits);
                out16 = sampleSquidge << (16 - crushBits);
                out[i] = out16 / 32767.0;
            }
        } else if (crushBits == 16) { // bitcrusher not being used, samplerate mods only.
            for (int i = 0; i < len;) {
                float sampleSqueeze = out[i];
                for (int j = 0; j < sampleStep && i < len; j++) {
                    // for each repeated sample, paste in the current
                    // root sample, then move onto the next step.
                    out[i] = sampleSqueeze;
                    i++;
                }
            }
        } else { // both being used. crush those bits and mash those samples.
            for (int i = 0; i < len;) {
                int16_t sampleSqueeze = out[i] * 32767;
                for (int j = 0; j < sampleStep && i < len; j++) {
                    int16_t sampleSquidge = sampleSqueeze >> (16 - crushBits);
                    out[i] = (sampleSquidge << (16 - crushBits)) / 32767.0;
                    i++;
                }
            }
        }
    }
};

#endif
