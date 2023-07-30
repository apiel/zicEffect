#ifndef _AUDIO_HANDLER_H_
#define _AUDIO_HANDLER_H_

#include "audioEffect.h"
#include "def.h"
#include "distortion.h"
#include "filter.h"

class AudioHandler {
protected:
    static AudioHandler* instance;

    AudioHandler()
    {
    }

public:
    Filter filter;
    Distortion distortion;
    AudioEffect audioEffect;

    uint8_t stepCounter = 0;

    static AudioHandler& get()
    {
        if (!instance) {
            instance = new AudioHandler();
        }
        return *instance;
    }

    void samples(float* buf, int len)
    {
        audioEffect.samples(buf, len);

        for (int i = 0; i < len; i++) {
            buf[i] = distortion.sample(filter.sample(buf[i])) * masterVolumeWithGain;
        }
    }
};

AudioHandler* AudioHandler::instance = NULL;

#endif