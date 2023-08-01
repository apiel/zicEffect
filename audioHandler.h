#ifndef _AUDIO_HANDLER_H_
#define _AUDIO_HANDLER_H_

#include "audioBuffer.h"
#include "def.h"
#include "distortion.h"
#include "filter.h"
// #include "granular.h"
#include "signalsmith-stretch/signalsmith-stretch.h"

class AudioHandler {
protected:
    static AudioHandler* instance;

    // Keep buffer for echo, delay, granular, etc.
    AudioBuffer buffer;

    signalsmith::stretch::SignalsmithStretch<float> stretch;

    AudioHandler()
    {
        stretch.presetDefault(APP_CHANNELS, SAMPLE_RATE);
        stretch.setTransposeSemitones(12);
    }

public:
    Filter filter;
    Distortion distortion;
    // Granular granular;

    uint8_t stepCounter = 0;

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

        stretch.process(&buffer.input, len, &out, len);

        // for (int i = 0; i < len; i++) {
        //     out[i] = bufferOut[i];
        // }

        // granular.samples(buffer, out, len);
    }
};

AudioHandler* AudioHandler::instance = NULL;

#endif