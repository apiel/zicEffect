#ifndef _SYNTH_GRANULAR_H_
#define _SYNTH_GRANULAR_H_

#include <sndfile.h>

#include "audioBuffer.h"
#include "def.h"
#include "effectGranular.h"

class SynthGranular {
protected:
    AudioBuffer<GRANULER_BUFFER_SECONDS> buffer;

public:
    EffectGranular granular;
    SF_INFO sfinfo;
    SNDFILE* file = NULL;

    SynthGranular()
    : granular(&buffer)
    {
        memset(&sfinfo, 0, sizeof(sfinfo));
        // open("samples/0_0_axxe13.wav");
    }

    ~SynthGranular()
    {
        close();
    }

    SynthGranular& close()
    {
        if (file) {
            sf_close(file);
        }
        return *this;
    }

    SynthGranular& open(const char* filename, int seek = SEEK_END)
    {
        close();

        if (!(file = sf_open(filename, SFM_READ, &sfinfo))) {
            APP_INFO("Error: could not open file %s\n", filename);
            return *this;
        }
        APP_INFO("Audio file %s sampleCount %ld sampleRate %d\n", filename, (long)sfinfo.frames, sfinfo.samplerate);

        sf_read_float(file, buffer.samples, buffer.size);

        return *this;
    }
};

#endif