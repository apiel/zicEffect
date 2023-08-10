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

class EffectFilterData {

public:
    float cutoff = 0.0f;
    float feedback = 0.0f;
    float buf0 = 0.0f;
    float buf1 = 0.0f;
    float hp = 0.0f;
    float resonance = 0.0f;

    void setCutoff(float _cutoff)
    {
        cutoff = _cutoff;
        setResonance(resonance);
    }

    void setResonance(float _resonance)
    {
        resonance = _resonance;
        if (resonance == 0.0f) {
            feedback = 0.0f;
            return;
        }
        feedback = resonance + resonance / (1.0 - cutoff);
    }

    void setSampleData(float inputValue)
    {
        hp = inputValue - buf0;
        float bp = buf0 - buf1;

        buf0 = buf0 + cutoff * (hp + feedback * bp);
        buf1 = buf1 + cutoff * (buf0 - buf1);
    }
};

class EffectFilter : public EffectFilterInterface {
protected:
    EffectFilterData hpf;
    EffectFilterData lpf;

    float mix = 0.5;

public:
    float resonance = 0.0;

    EffectFilter()
    {
        set(0.5);
    };

    float sample(float inputValue)
    {
        if (inputValue == 0) {
            return inputValue;
        }

        hpf.setSampleData(inputValue);
        lpf.setSampleData(inputValue);

        return lpf.buf1 * (1.0 - mix) + hpf.hp * mix;
    }

    EffectFilter& set(float value)
    {
        mix = range(value, 0.00, 1.00);

        hpf.setCutoff((0.20 * value) + 0.00707);
        lpf.setCutoff(0.85 * value + 0.1);

        return *this;
    }

    EffectFilter& setResonance(float _res)
    {
        resonance = range(_res, 0.00, 0.99);
        lpf.setResonance(resonance);
        hpf.setResonance(resonance);

        debug("Filter: resonance=%f\n", resonance);

        return *this;
    };
};

// another version of the same filter but with a small clicking at 0.5
class EffectFilter2 : public EffectFilterInterface {
protected:
    // cutoff cannot be 1.0 else div by zero range(_cutoff, 0.01, 0.99);
    float cutoff = 0.99;
    float feedback;
    float mix = 0.5;

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

        debug("Mix (%f): cutoff=%f reso %f feedback %f\n", mix, cutoff, _resonance, feedback);
    }

    float sample(float inputValue, float _cutoff)
    {
        if (inputValue == 0) {
            return inputValue;
        }

        float hp = inputValue - buf0;
        float bp = buf0 - buf1;

        buf0 = buf0 + _cutoff * (hp + feedback * bp);
        buf1 = buf1 + _cutoff * (buf0 - buf1);

        return buf1 * (1.0 - mix) + hp * mix;
    }

public:
    float resonance = 0.0;

    EffectFilter2()
    {
        set(0.5);
    };

    float sample(float inputValue)
    {
        return sample(inputValue, cutoff);
    }

    EffectFilter2& set(float value)
    {
        mix = range(value, 0.00, 1.00);

         if (value > 0.5) {
            // 0 to 0.10
            cutoff = (0.10 * ((value - 0.5) * 2)) + 0.00707;
        } else {
            // From 0.95 to 0.1
            cutoff = 0.85 * (value * 2) + 0.1;
        }

        // if (value > 0.5) {
        //     cutoff = 1 - value + 0.0707;
        // } else {
        //     cutoff = value + 0.05; // LPF should not be 0.0
        // }

        // debug("Filter (%f): cutoff=%f\n", value, cutoff);
        calculateVar();
        return *this;
    }

    EffectFilter2& setResonance(float _res)
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
        p = _cutoff + 0.8f * _cutoff * q;
        f = p + p - 1.0f;
        q = _resonance * (1.0f + 0.5f * q * (1.0f - q + 5.6f * q * q));

        // debug("mix %f cutoff %f q=%f\n", mix, _cutoff, q);
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
        if (mix > 0.5) {
            cutoff = 1 - mix + 0.0707;
        } else {
            cutoff = mix + 0.05; // LPF should not be 0.0
        }
        calculateVar(cutoff, resonance);
        return *this;
    }

    EffectFilterMoog& setResonance(float _res)
    {
        resonance = range(_res, 0.00, 1.00) * 0.90; // resonance should be from 0.0 to 0.90, higher values is too high
        calculateVar(cutoff, resonance);
        return *this;
    };
};

#endif
