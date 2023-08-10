#ifndef _EFFECT_FILTER_H_
#define _EFFECT_FILTER_H_

#include "def.h"

#include <math.h>

class EffectFilterInterface {
public:
    float resonance = 0.0;

    virtual float sample(float inputValue) = 0;
    virtual EffectFilterInterface& set(float value) = 0;
    virtual EffectFilterInterface& setResonance(float _res) = 0;
};

// https://www.martin-finke.de/articles/audio-plugins-013-filter/
// https://www.musicdsp.org/en/latest/Filters/29-resonant-filter.html

class EffectFilter : public EffectFilterInterface {
protected:
    enum FilterMode {
        FILTER_MODE_OFF,
        FILTER_MODE_LOWPASS_12,
        FILTER_MODE_HIGHPASS_12,
        FILTER_MODE_COUNT,
    };

    // cutoff cannot be 1.0 else div by zero range(_cutoff, 0.01, 0.99);
    float cutoff = 0.99;
    float feedback;

    // NOTE Should we take care of channel separation?
    float buf0 = 0;
    float buf1 = 0;

    void calculateVar()
    {
        calculateVar(cutoff, resonance);
    }

    void calculateVar(float _cutoff, float _resonance)
    {
        if (_resonance == 0.0f) {
            feedback = 0.0f;
            return;
        }

        // cutoff cannot be 1.0 (should we ensure this?)
        feedback = _resonance + _resonance / (1.0 - _cutoff); // would it make sense to make a lookup table for `1.0 / (1.0 - _cutoff);` ?
    }

    float sample(float inputValue, float _cutoff)
    {
        if (mode == FILTER_MODE_OFF || inputValue == 0) {
            return inputValue;
        }

        float hp = inputValue - buf0;
        float bp = buf0 - buf1;

        buf0 = buf0 + _cutoff * (hp + feedback * bp);
        buf1 = buf1 + _cutoff * (buf0 - buf1);

        if (mode == FILTER_MODE_LOWPASS_12) {
            return buf1;
        }
        return hp;
    }

public:
    float resonance = 0.0;
    uint8_t mode = FILTER_MODE_OFF;

    EffectFilter()
    {
        set(0.5);
    };

    float sample(float inputValue)
    {
        return sample(inputValue, cutoff);
    }

    EffectFilter& set(float value)
    {
        if (value == 0.5) {
            mode = FILTER_MODE_OFF;
        } else if (value > 0.5) {
            mode = FILTER_MODE_HIGHPASS_12;
            // 0 to 0.10
            cutoff = (0.10 * ((value - 0.5) * 2)) + 0.00707;
        } else {
            mode = FILTER_MODE_LOWPASS_12;
            // From 0.95 to 0.1
            cutoff = 0.85 * (value * 2) + 0.1;
        }

        debug("Filter (%f): cutoff=%f mode=%d\n", value, cutoff, mode);
        calculateVar();
        return *this;
    }

    EffectFilter& setResonance(float _res)
    {
        resonance = range(_res, 0.00, 0.99);
        calculateVar();

        debug("Filter: resonance=%f\n", resonance);

        return *this;
    };
};

// Paul Kellet version of the classic Stilson/Smith "Moog" filter
// https://www.kvraudio.com/forum/viewtopic.php?t=144625
class EffectFilterMoog : public EffectFilterInterface {
protected:
    float cutoff = 0.00;
    float f, p, q = 0.00;
    float b0, b1, b2, b3, b4 = 0.0;
    float t1, t2 = 0.0;
    float mix = 0.5;

    void calculateVar(float _cutoff, float _resonance)
    {
        q = 1.0f - _cutoff;
        // if (mix > 0.5) {
        //     q = 1.0f - mix;
        // }
        p = _cutoff + 0.8f * _cutoff * q;
        f = p + p - 1.0f;
        // q = _resonance * (1.0f + 0.5f * q * (1.0f - q + 5.6f * q * q));

        q = _resonance * (1.0f + 0.5f * q * (1.0f - q + 5.6f * q * q));

        // q = _resonance * 2;

        printf("mix %f cutoff %f q=%f\n", mix, _cutoff, q);
    }

public:
    float resonance = 0.0;

    EffectFilterMoog()
    {
        set(0.5);
    };

    float sample(float inputValue)
    {
        inputValue -= q * b4; // feedback
        t1 = b1;
        b1 = (inputValue + b0) * p - b1 * f;
        t2 = b2;
        b2 = (b1 + t1) * p - b2 * f;
        t1 = b3;
        b3 = (b2 + t2) * p - b3 * f;
        b4 = (b3 + t1) * p - b4 * f;
        b4 = b4 - b4 * b4 * b4 * 0.166667f; // clipping
        b0 = inputValue;

        // Lowpass  output:  b4
        // Highpass output:  in - b4;
        // Bandpass output:  3.0f * (b3 - b4);

        return b4 * (1.0 - mix) + (inputValue - b4) * mix;
    }

    EffectFilterMoog& set(float value)
    {
        mix = range(value, 0.00, 1.00);
        if (value > 0.5) {
            cutoff = 1 - value + 0.0707;
        } else {
            cutoff = value + 0.01; // LPF should not be 0.0
        }
        calculateVar(cutoff, resonance);

        // printf("LPF %f HPF %f\n", (1.0 - mix), mix);
        return *this;
    }

    EffectFilterMoog& setResonance(float _res)
    {
        resonance = range(_res, 0.00, 1.00) * 0.85; // resonance should be from 0.0 to 0.85, higher values is too high
        calculateVar(cutoff, resonance);
        return *this;
    };
};

#endif
