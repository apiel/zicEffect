#ifndef _PITCH_SHIFTER_H_
#define _PITCH_SHIFTER_H_

#include "audioBuffer.h"
#include "def.h"
#include "signalsmith-stretch/signalsmith-stretch.h"

// still not so good maybe better to pitch down directly the gear with midi
// seem we can pitch bend digitone with [225,39,100] and [225,39,0]

class PitchShifter {
protected:
    float buffer[APP_AUDIO_CHUNK * APP_CHANNELS];
    float input[APP_AUDIO_CHUNK * APP_CHANNELS];

public:
    bool on = false;
    float ramp = 0.0;

    signalsmith::stretch::SignalsmithStretch<float> stretch;

    PitchShifter()
    {
        stretch.presetCheaper(APP_CHANNELS, SAMPLE_RATE);
        stretch.setTransposeSemitones(-2);
    }

    PitchShifter& setTransposeSemitones(int8_t semitones)
    {
        stretch.setTransposeSemitones(semitones);
        return *this;
    }

    void samples(float* out, int len)
    {        
        for (int i = 0; i < len; i++) {
            buffer[i] = input[i];
        }

        if (on) {
            stretch.process(&out, len, &buffer, len);
            if (ramp < 1.0) {
                ramp += 0.01;
            }
            for (int i = 0; i < len; i++) {
                out[i] = buffer[i] * ramp + input[i] * (1 - ramp);
            }
        } else {
            if (ramp > 0.0) {
                ramp -= 0.01;
                stretch.process(&out, len, &buffer, len);
                for (int i = 0; i < len; i++) {
                    out[i] = buffer[i] * ramp + input[i] * (1 - ramp);
                }
            }
        }
    }
};

#endif