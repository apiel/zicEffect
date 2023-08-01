#ifndef _GRANULAR_H_
#define _GRANULAR_H_

#include <cstdlib>
using namespace std;

#include "def.h"
#include "audioBuffer.h"

#define MAX_GRAINS_PER_VOICE 24
#define MAX_GRAIN_VOICES 4
#define MIN_GRAIN_SIZE_MS 20

const uint16_t minGrainSampleCount = MIN_GRAIN_SIZE_MS * SAMPLE_RATE * 0.001f;

class Granular {
protected:
    uint64_t voicePosition = 0;
    float attackStep = 0.0f;
    float releaseStep = 0.0f;

    struct Grain {
        float pos;
        int64_t start;
        int64_t sampleCount;
        int64_t delay;
        float sampleStep;
    };

    struct Voice {
        int8_t note = -1;
        Grain grains[MAX_GRAINS_PER_VOICE];
        float (Granular::*envelop)(Voice& voice) = &Granular::envelopSustain;
        float env = 0.0f;
        uint64_t position = 0;
    } voices[MAX_GRAIN_VOICES];

    uint8_t baseNote = 60;
    float getSampleStep(uint8_t note)
    {
        // https://gist.github.com/YuxiUx/ef84328d95b10d0fcbf537de77b936cd
        // pow(2, ((note - 0) / 12.0)) = 1 for 0 semitone
        // pow(2, ((note - 1) / 12.0)) = 1.059463094 for 1 semitone
        // pow(2, ((note - 2) / 12.0)) = 1.122462048 for 2 semitone
        // ...
        return pow(2, ((note - baseNote) / 12.0));
    }

    void initGrain(Grain& grain)
    {
        initGrain(grain, grain.sampleStep);
    }

    int getRand()
    {
        // could create a lookup table ?
        srand(time(0));
        return rand();
    }

    float getRandPct()
    {
        return getRand() / (float)RAND_MAX;
    }

    void initGrain(Grain& grain, float sampleStep)
    {
        grain.sampleStep = sampleStep;
        grain.pos = 0.0f;

        // sprayToAdd is a random value between 0 and spray from starting point till end of file
        float sprayToAdd = spray ? (getRandPct() * spray * (1 - start)) : 0.0;
        grain.start = (start + sprayToAdd) * AUDIO_BUFFER_SIZE;

        // we deduct minGrainSampleCount to avoid grainSize to be too small
        grain.sampleCount = (AUDIO_BUFFER_SIZE - (grain.start + minGrainSampleCount)) * grainSize + minGrainSampleCount;

        grain.delay = delay ? (getRand() % delay) : 0;
    }

    float envelopAttack(Voice& voice)
    {
        voice.env += attackStep;
        if (voice.env >= 1.0f) {
            voice.env = 1.0f;
            voice.envelop = &Granular::envelopSustain;
            // debug("envelopAttack finished, set env to %f\n", voice.env);
        }
        return voice.env;
    }

    float envelopSustain(Voice& voice)
    {
        return voice.env;
    }

    float envelopRelease(Voice& voice)
    {
        voice.env -= releaseStep;
        if (voice.env <= 0.0f) {
            voice.env = 0.0f;
            voice.note = -1;
            // debug("envelopRelease finished, set env to %f\n", voice.env);
        }
        return voice.env;
    }

    float sample(AudioBuffer& buffer, Voice& voice)
    {
        float sample = 0.0f;
        float env = (this->*voice.envelop)(voice);
        if (env <= 0.0f) {
            return sample;
        }

        for (uint8_t d = 0; d < density; d++) {
            Grain& grain = voice.grains[d];
            if (grain.delay > 0) {
                grain.delay--;
            } else {
                int64_t samplePos = (uint64_t)grain.pos + grain.start;
                // if (samplePos < AUDIO_BUFFER_SIZE && (int64_t)grain.pos < grainSampleCount) { // is samplePos < AUDIO_BUFFER_SIZE even necessary if start calculated properly
                if ((int64_t)grain.pos < grain.sampleCount) {
                    grain.pos += grain.sampleStep;
                    sample += buffer.data[samplePos] * env;
                } else {
                    initGrain(grain);
                }
            }
        }
        return sample;
    }

    Voice& getNextVoice(uint8_t note)
    {
        // First, we should look if the voice is not already running, due to the envelopRelease
        for (uint8_t v = 0; v < MAX_GRAIN_VOICES; v++) {
            Voice& voice = voices[v];
            if (voice.note == note) {
                debug("getNextVoice: voice already running %d\n", note);
                return voice;
            }
        }

        // Else, we should look for a free voice
        uint8_t voiceToSteal = 0;
        for (uint8_t v = 0; v < MAX_GRAIN_VOICES; v++) {
            Voice& voice = voices[v];
            if (voice.note == -1) {
                return voice;
            }
            if (voice.position < voices[voiceToSteal].position) {
                voiceToSteal = v;
            }
        }

        debug("getNextVoice: no voice available. Steal voice %d.\n", voiceToSteal);
        return voices[voiceToSteal];
    }

public:
    uint8_t density = 10;
    float start = 0.0f;
    float grainSize = 0.1;
    float spray = 0.5;
    uint64_t delay = 0;
    uint16_t attack = 20;
    uint16_t release = 50;

    Granular()
    {
        setGrainSize(grainSize);
        setAttack(attack);
        setRelease(release);
    }

    /**
     * @brief Set the Grain Size meaning the length duration of the grain.
     *
     * @param grainSize
     * @return Granular&
     */
    Granular& setGrainSize(float value)
    {
        grainSize = range(value, 0.0, 1.0);
        debug("grainSize %f\n", grainSize);
        return *this;
    }

    /**
     * @brief Set the Spray of the grain start position, giving random position to
     * the grain start position.
     *
     * @param spray
     * @return Granular&
     */
    Granular& setSpray(float value)
    {
        spray = range(value, 0.0, 1.0);
        debug("spray %f\n", spray);
        return *this;
    }

    /**
     * @brief Set the Density meaning the number of grains that are played at the same time.
     *
     * @param density
     * @return Granular&
     */
    Granular& setDensity(float value)
    {
        density = value * (MAX_GRAINS_PER_VOICE - 1) + 1; // 1 to MAX_GRAINS_PER_VOICE
        debug("density %d\n", density);
        return *this;
    }

    /**
     * @brief Set the Start position of the sample to play
     *
     * @param _start position from 0.0 to 1.0, where 0.0 is the start of the sample
     * and 1.0 the end of the sample
     * @return Granular&
     */
    Granular& setStart(float value)
    {
        start = range(value, 0.0f, 1.0f);
        debug("setStart %f\n", start);
        return *this;
    }

    /**
     * @brief Set the Delay before grain start to play
     *
     * @param delay where 0 is no delay and 1 is 1000ms
     * @return Granular&
     */
    Granular& setDelay(float value)
    {
        // delay = range(value, 0.0f, 1.0f) * SAMPLE_RATE * 0.001f * 1000;
        // can be simplified to:
        delay = range(value, 0.0f, 1.0f) * SAMPLE_RATE;
        debug("delay %ld\n", delay);
        return *this;
    }

    /**
     * @brief Set the Attack time of the voice
     *
     * @param attack where 0 is no attack and 1 is 5000ms
     * @return Granular&
     */
    Granular& setAttack(float value)
    {
        // uint64_t attackSamples = range(value, 0.0f, 1.0f) * SAMPLE_RATE * 0.001f * 5000;
        // can be simplified to:
        uint64_t attackSamples = range(value, 0.0f, 1.0f) * SAMPLE_RATE * 5;
        attackStep = 1.0f / attackSamples;
        debug("attack %ld samples %f step\n", attackSamples, attackStep);
        return *this;
    }

    /**
     * @brief Set the Release time of the voice
     *
     * @param release where 0 is no release and 1 is 10000ms
     * @return Granular&
     */
    Granular& setRelease(float value)
    {
        // uint64_t releaseSamples = range(value, 0.0f, 1.0f) * SAMPLE_RATE * 0.001f * 10000;
        // can be simplified to:
        uint64_t releaseSamples = range(value, 0.0f, 1.0f) * SAMPLE_RATE * 10;
        releaseStep = 1.0f / releaseSamples;
        debug("release %ld samples %f step\n", releaseSamples, releaseStep);
        return *this;
    }

    int64_t samples(AudioBuffer& buffer,float* buf, int len)
    {
        int i = 0;

        // for(; i < len; i++) {
        //     buf[i] = 0;
        // }

        for (uint8_t v = 0; v < MAX_GRAIN_VOICES; v++) {
            Voice& voice = voices[v];
            if (voice.note != -1) {
                for (i = 0; i < len; i++) {
                    // buf[i] += sample(buffer, voice);
                    // printf("buf[%d] = %f\n", i, buf[i]);
                    // buf[i] = (buf[i] * 0.8) + (sample(buffer, voice) * 0.2);
                    buf[i] = sample(buffer, voice);
                }
            }
        }

        return i;
    }

    Granular& noteOn(uint8_t note, uint8_t velocity)
    {
        if (velocity == 0) {
            return noteOff(note, velocity);
        }

        Voice& voice = getNextVoice(note);
        voice.position = voicePosition++;
        voice.note = note;
        voice.envelop = &Granular::envelopAttack;
        float sampleStep = getSampleStep(note);
        for (uint8_t g = 0; g < density; g++) {
            initGrain(voice.grains[g], sampleStep);
        }
        debug("noteOn: %d %d %f\n", note, velocity, sampleStep);

        return *this;
    }

    Granular& noteOff(uint8_t note, uint8_t velocity)
    {
        for (uint8_t v = 0; v < MAX_GRAIN_VOICES; v++) {
            Voice& voice = voices[v];
            if (voice.note == note) {
                voice.envelop = &Granular::envelopRelease;
                debug("noteOff set on to false: %d %d\n", note, velocity);
                return *this;
            }
        }

        debug("noteOff: note not found %d %d\n", note, velocity);
        return *this;
    }
};

#endif