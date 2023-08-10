#ifndef _EFFECT_DELAY_H_
#define _EFFECT_DELAY_H_

#include "def.h"
#include "audioBuffer.h"

class EffectDelay {
protected:
    AudioBuffer* buffer;
    uint32_t delayIndex = 0;

public:
    float sec = 0.5f;
    float amplitude = 0.2f;
    float feedback = 0.0f;

    EffectDelay(AudioBuffer* _buffer)
        : buffer(_buffer)
    {
        // To ensure that default sets values respect range
        set(sec, amplitude, feedback);
    }

    /**
     * @brief Set the delay time (in sec), amplitude and feedback
     *
     * @param _sec
     * @param _amplitude
     * @param _feedback
     */
    void set(float _sec, float _amplitude, float _feedback)
    {
        setSec(_sec);
        setAmplitude(_amplitude);
        setFeedback(_feedback);
    }

    /**
     * @brief Set the delay time in seconds
     *
     * @param _sec
     * @return EffectDelay*
     */
    EffectDelay* setSec(float _sec)
    {
        sec = range(_sec, 0.01, (float)AUDIO_BUFFER_SECONDS);
        delayIndex = (buffer->index + AUDIO_BUFFER_SIZE - (uint32_t)(SAMPLE_RATE * sec)) % AUDIO_BUFFER_SIZE;
        return this;
    }

    /**
     * @brief Set the amplitude
     *
     * @param _amplitude
     * @return EffectDelay*
     */
    EffectDelay* setAmplitude(float _amplitude)
    {
        amplitude = range(_amplitude, 0.0f, 1.0f);
        return this;
    }

    /**
     * @brief Set the feedback
     *
     * @param _feedback
     * @return EffectDelay*
     */
    EffectDelay* setFeedback(float _feedback)
    {
        feedback = range(_feedback, 0.0f, 1.0f);
        return this;
    }

    /**
     * @brief return the delayed sample
     *
     * @return float
     */
    float sample()
    {
        if (amplitude == 0.0) {
            return 0;
        }
        // float delay = (buffer->samples[(buffer->index + AUDIO_BUFFER_SIZE - (uint32_t)(SAMPLE_RATE * sec)) % AUDIO_BUFFER_SIZE] * amplitude);

        if (delayIndex++ >= AUDIO_BUFFER_SIZE) {
            delayIndex = 0;
        }
        float delay = (buffer->samples[delayIndex] * amplitude);

        if (feedback > 0.0) {
            buffer->samples[buffer->index] += delay * feedback;
        }
        return delay;
    }
};

#endif