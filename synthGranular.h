#ifndef _SYNTH_GRANULAR_H_
#define _SYNTH_GRANULAR_H_

#include <sndfile.h>

#include "audioBuffer.h"
#include "def.h"
#include "effectGranular.h"
#include "fileBrowser.h"

// class SynthGranular {
// protected:
//     AudioBuffer<GRANULER_BUFFER_SECONDS> buffer;

// public:
//     EffectGranular granular;
//     SF_INFO sfinfo;
//     SNDFILE* file = NULL;

//     SynthGranular()
//     : granular(&buffer)
//     {
//         memset(&sfinfo, 0, sizeof(sfinfo));
//         // open("samples/0_0_axxe13.wav");
//     }

//     ~SynthGranular()
//     {
//         close();
//     }

//     SynthGranular& close()
//     {
//         if (file) {
//             sf_close(file);
//         }
//         return *this;
//     }

//     SynthGranular& open(const char* filename, int seek = SEEK_END)
//     {
//         close();

//         if (!(file = sf_open(filename, SFM_READ, &sfinfo))) {
//             APP_INFO("Error: could not open file %s\n", filename);
//             return *this;
//         }
//         APP_INFO("Audio file %s sampleCount %ld sampleRate %d\n", filename, (long)sfinfo.frames, sfinfo.samplerate);

//         sf_read_float(file, buffer.samples, buffer.size);

//         return *this;
//     }

//     float sample()
//     {
//         return granular.sample();
//     }
// };

class SynthGranular : public EffectGranular {
protected:
    AudioBuffer<GRANULER_BUFFER_SECONDS> buffer;
    FileBrowser fileBrowser = FileBrowser("./samples");

public:
    SF_INFO sfinfo;
    SNDFILE* file = NULL;

    SynthGranular()
        : EffectGranular(&buffer)
    {
        memset(&sfinfo, 0, sizeof(sfinfo));
        open(0.0);
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

    SynthGranular& open(float value)
    {
        uint8_t position = range(value * 127, 0, fileBrowser.count);
        if (position != fileBrowser.position) {
            char* file = fileBrowser.getFile(position);
            debug("SAMPLE_SELECTOR: %f %s\n", value, file);
            open(file);
        }
        return *this;
    }
};

#endif