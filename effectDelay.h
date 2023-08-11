#ifndef _EFFECT_DELAY_H_
#define _EFFECT_DELAY_H_

#include "audioBuffer.h"
#include "def.h"
#include "effectFilter.h"

#define MAX_DELAY_VOICES 16

// TODO load/save different kind of delay and reverb from a config file
// TODO add filter on voices or on master???
// TODO add lfo on delay time

class EffectDelay {
protected:
    AudioBuffer* buffer;

    struct DelayVoice {
        uint32_t index;
        float amplitude;
        float feedback;
        float sec;
    } voices[MAX_DELAY_VOICES] = {
        { 0, 0.0f, 0.0f, 0.1f },
    };

public:
    // From 0.0 to 1.0 to apply time ratio to voice in seconds
    float timeRatio = 1.0f;
    float masterAmplitude = 1.0f;

    EffectFilter filter;

    EffectDelay(AudioBuffer* _buffer)
        : buffer(_buffer)
    {
        setVoice(0, 0.1f, 0.6f, 0.0f);
        setVoice(1, 0.2f, 0.5f, 0.0f);
        setVoice(2, 0.3f, 0.4f, 0.0f);
        setVoice(3, 0.4f, 0.3f, 0.0f);
        setVoice(4, 0.5f, 0.2f, 0.0f);

        filter.setCutoff(0.5f).setResonance(0.95f).setMode(EffectFilter::Mode::HPF);

        // // make reverb
        // setVoice(0, 0.01f, 0.9f, 0.0f);
        // setVoice(1, 0.02f, 0.85f, 0.0f);
        // setVoice(2, 0.03f, 0.8f, 0.0f);
        // setVoice(3, 0.004f, 0.75f, 0.0f);
        // setVoice(4, 0.05f, 0.7f, 0.0f);
        // setVoice(5, 0.06f, 0.65f, 0.0f);
        // setVoice(6, 0.07f, 0.6f, 0.0f);
        // setVoice(7, 0.08f, 0.55f, 0.0f);
        // setVoice(8, 0.09f, 0.5f, 0.0f);
        // setVoice(9, 0.10f, 0.45f, 0.0f);
        // setVoice(10, 0.11f, 0.4f, 0.0f);
        // setVoice(11, 0.12f, 0.35f, 0.0f);
        // setVoice(12, 0.13f, 0.3f, 0.0f);
        // setVoice(13, 0.14f, 0.25f, 0.0f);
        // setVoice(14, 0.15f, 0.2f, 0.0f);
        // setVoice(15, 0.16f, 0.15f, 0.0f);
    }

    EffectDelay* setSec(uint8_t voiceIndex, float sec)
    {
        voices[voiceIndex].sec = sec;
        voices[voiceIndex].index = (buffer->index + AUDIO_BUFFER_SIZE - (uint32_t)(SAMPLE_RATE * sec * timeRatio)) % AUDIO_BUFFER_SIZE;
        return this;
    }

    EffectDelay* setAmplitude(uint8_t voiceIndex, float amplitude)
    {
        voices[voiceIndex].amplitude = range(amplitude, 0.0f, 1.0f);
        return this;
    }

    EffectDelay* setFeedback(uint8_t voiceIndex, float feedback)
    {
        voices[voiceIndex].feedback = range(feedback, 0.0f, 1.0f);
        return this;
    }

    EffectDelay* setFeedback(float feedback)
    {
        for (uint8_t i = 0; i < MAX_DELAY_VOICES; i++) {
            setFeedback(i, feedback);
        }
        return this;
    }

    EffectDelay* setVoice(uint8_t voiceIndex, float sec, float amplitude, float feedback)
    {
        setSec(voiceIndex, sec);
        setAmplitude(voiceIndex, amplitude);
        setFeedback(voiceIndex, feedback);
        return this;
    }

    EffectDelay* setMasterAmplitude(float amplitude)
    {
        masterAmplitude = range(amplitude, 0.0f, 1.0f);
        return this;
    }

    EffectDelay* setTimeRatio(float ratio)
    {
        timeRatio = range(ratio, 0.0f, 1.0f);
        for (uint8_t i = 0; i < MAX_DELAY_VOICES; i++) {
            setSec(i, voices[i].sec);
        }
        return this;
    }

    float sample()
    {
        float delay = 0.0f;
        for (uint8_t i = 0; i < MAX_DELAY_VOICES; i++) {
            DelayVoice& voice = voices[i];
            if (voice.index++ >= AUDIO_BUFFER_SIZE) {
                voice.index = 0;
            }
            if (masterAmplitude && voice.amplitude > 0.0f) {
                delay += buffer->samples[voice.index] * voice.amplitude * masterAmplitude;
                if (voice.feedback > 0.0f) {
                    buffer->samples[buffer->index] += delay * voice.feedback;
                }
            }
        }

        return filter.sample(delay);
    }
};

#endif