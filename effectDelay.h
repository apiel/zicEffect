#ifndef _EFFECT_DELAY_H_
#define _EFFECT_DELAY_H_

#include "audioBuffer.h"
#include "def.h"

#define MAX_DELAY_VOICES 16

class EffectDelay {
protected:
    AudioBuffer* buffer;
    uint32_t delayIndex = 0;

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

    EffectDelay(AudioBuffer* _buffer)
        : buffer(_buffer)
    {
        setVoice(0, 0.1f, 0.6f, 0.0f);
        setVoice(1, 0.2f, 0.5f, 0.0f);
        setVoice(2, 0.3f, 0.4f, 0.0f);
        setVoice(3, 0.4f, 0.3f, 0.0f);
        setVoice(4, 0.5f, 0.2f, 0.0f);
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
            if (voice.amplitude > 0.0f) {
                delay += buffer->samples[voice.index] * voice.amplitude * masterAmplitude;
                if (voice.feedback > 0.0f) {
                    buffer->samples[buffer->index] += delay * voice.feedback;
                }
            }
        }
        return delay;
    }
};

#endif