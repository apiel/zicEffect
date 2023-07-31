#ifndef _FILTER_H_
#define _FILTER_H_

#include "def.h"

#include <math.h>

// https://www.martin-finke.de/articles/audio-plugins-013-filter/
// https://www.musicdsp.org/en/latest/Filters/29-resonant-filter.html

class Filter {
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

        buf0 += _cutoff * (inputValue - buf0 + feedback * (buf0 - buf1));
        buf1 += _cutoff * (buf0 - buf1);

        if (mode == FILTER_MODE_LOWPASS_12) {
            return buf1;
        }
        return inputValue - buf0;
    }

public:
    float resonance = 0.0;
    uint8_t mode = FILTER_MODE_OFF;

    Filter()
    {
        set(0.5);
    };

    float sample(float inputValue)
    {
        return sample(inputValue, cutoff);
    }

    Filter& set(float value)
    {
        if (value == 0.5) {
            mode = FILTER_MODE_OFF;
        } else if (value > 0.5) {
            mode = FILTER_MODE_HIGHPASS_12;
            // 0 to 0.10
            cutoff = 0.10 * ((value - 0.5) * 2);
        } else {
            mode = FILTER_MODE_LOWPASS_12;
            // From 0.95 to 0.1
            cutoff = 0.85 * (value * 2) + 0.1;
        }

        debug("Filter (%f): cutoff=%f\n", value, cutoff);
        calculateVar();
        return *this;
    }

    Filter& setResonance(float _res)
    {
        resonance = range(_res, 0.00, 0.99);
        calculateVar();

        debug("Filter: resonance=%f\n", resonance);

        return *this;
    };
};

#endif
