#ifndef _AUDIO_HANDLER_H_
#define _AUDIO_HANDLER_H_

#include "audioBuffer.h"
#include "def.h"
#include "distortion.h"
#include "filter.h"
#include "pitchShifter.h"

class AudioHandler {
protected:
    static AudioHandler* instance;

    // Keep buffer for echo, delay, granular, etc.
    AudioBuffer buffer;

    AudioHandler()
    {
    }

public:
    Filter filter;
    Distortion distortion;
    PitchShifter pitchShifter;

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
            out[i] = distortion.sample(filter.sample(in[i]));
            buffer.addSample(out[i], i);
        }

        // if (pitchShifter.on) {
        //     pitchShifter.stretch.process(&buffer.input, len, &out, len);
        //     if (pitchShifter.ramp < 1.0) {
        //         pitchShifter.ramp += 0.01;
        //     }
        //     for (int i = 0; i < len; i++) {
        //         out[i] = out[i] * pitchShifter.ramp + buffer.input[i] * (1 - pitchShifter.ramp);
        //     }
        // } else {
        //     if (pitchShifter.ramp > 0.0) {
        //         pitchShifter.ramp -= 0.01;
        //         pitchShifter.stretch.process(&buffer.input, len, &out, len);
        //         for (int i = 0; i < len; i++) {
        //             out[i] = out[i] * pitchShifter.ramp + buffer.input[i] * (1 - pitchShifter.ramp);
        //         }
        //     }
        // }

        pitchShifter.samples(out, len);
    }
};

AudioHandler* AudioHandler::instance = NULL;

#endif