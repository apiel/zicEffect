#ifndef _AUDIO_API_H_
#define _AUDIO_API_H_

#include "audio.h"
#include "audioHandler.h"
#include "def.h"

AudioHandler& audioHandler = AudioHandler::get();

int audioCallback(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
    double /*streamTime*/, RtAudioStreamStatus status, void* data)
{
    // audioHandler.samples((float*)outputBuffer, nBufferFrames * APP_CHANNELS);

    // float* buf = (float*)inputBuffer;
    // for (int i = 0; i < nBufferFrames * APP_CHANNELS; i++) {
    //     printf("%f\n", buf[i]);
    // }

    for (int i = 0; i < nBufferFrames * APP_CHANNELS; i++) {
        ((float*)outputBuffer)[i] = ((float*)inputBuffer)[i];
    }
    return 0;
}

int audioInputCallback(void* /*outputBuffer*/, void* inputBuffer, unsigned int nBufferFrames,
    double /*streamTime*/, RtAudioStreamStatus status, void* data)
{
    // audioHandler.inputSamples((float*)inputBuffer, nBufferFrames * APP_CHANNELS);

    // float* buf = (float*)inputBuffer;
    // printf("nBufferFrames: %d\n", nBufferFrames);
    // float yo = buf[0];
    // // printf("%f\n", buf[0]);

    // for (int i = 0; i < nBufferFrames * APP_CHANNELS; i++) {
    //     printf("%f\n", buf[i]);
    // }

    return 0;
}

class AudioRT : public AudioApi {
protected:
    RtAudio* audioOutput = 0;
    RtAudio* audioInput = 0;

    static AudioRT* instance;
    AudioRT()
    {
        audioOutput = new RtAudio();
        audioInput = new RtAudio();
    }

    const char* getApiName()
    {
        switch (audioOutput->getCurrentApi()) {
        case RtAudio::UNSPECIFIED:
            return "UNSPECIFIED";
        case RtAudio::LINUX_ALSA:
            return "LINUX_ALSA";
        case RtAudio::LINUX_PULSE:
            return "LINUX_PULSE";
        case RtAudio::LINUX_OSS:
            return "LINUX_OSS";
        case RtAudio::UNIX_JACK:
            return "UNIX_JACK";
        case RtAudio::MACOSX_CORE:
            return "MACOSX_CORE";
        case RtAudio::WINDOWS_WASAPI:
            return "WINDOWS_WASAPI";
        case RtAudio::WINDOWS_ASIO:
            return "WINDOWS_ASIO";
        case RtAudio::WINDOWS_DS:
            return "WINDOWS_DS";
        case RtAudio::RTAUDIO_DUMMY:
            return "RTAUDIO_DUMMY";
        default:
            return "?";
        }
    }

    void showAudioDeviceInfo()
    {
        unsigned int deviceCount = audioOutput->getDeviceCount();

        APP_PRINT("Found %d audio devices (%s):\n\n", deviceCount, getApiName());
        RtAudio::DeviceInfo rtInfo;
        for (unsigned int i = 0; i < deviceCount; i++) {
            rtInfo = audioOutput->getDeviceInfo(i);
            if (rtInfo.probed == true) {
                APP_PRINT(" (%d) %s\n", i, rtInfo.name.c_str());
                if (rtInfo.outputChannels) {
                    APP_PRINT("   [AUDIO_OUTPUT=%s]\n", rtInfo.name.c_str());
                }
                if (rtInfo.inputChannels) {
                    APP_PRINT("   [AUDIO_INPUT=%s]\n", rtInfo.name.c_str());
                }
                APP_PRINT("\n");
            }
        }
    }

    unsigned int getAudioDeviceId(char* name)
    {
        unsigned int deviceCount = audioOutput->getDeviceCount();

        RtAudio::DeviceInfo rtInfo;
        for (unsigned int i = 0; i < deviceCount; i++) {
            rtInfo = audioOutput->getDeviceInfo(i);
            if (rtInfo.probed == true) {
                if (rtInfo.name.find(name) != std::string::npos) {
                    return i;
                }
            }
        }
        debug("/!\\ Warning /!\\ Audio device %s not found\n", name);
        rtInfo = audioOutput->getDeviceInfo(0);
        debug("Using default %s device [%s]\n", getApiName(), rtInfo.name.c_str());
        return 0;
    }

public:
    static AudioRT& get()
    {
        if (!instance) {
            instance = new AudioRT();
        }
        return *instance;
    }

    int open()
    {
        APP_PRINT("RT audio::open\n");

        RtAudio::StreamParameters audioOutputParams;
        RtAudio::StreamParameters audioInputParams;

        // TODO should sample rate come from RtAudio::DeviceInfo  ?
        unsigned int bufferFrames = APP_AUDIO_CHUNK;
        audioOutputParams.deviceId = getAudioDeviceId(audioOutputName);
        audioOutputParams.nChannels = APP_CHANNELS;
        audioInputParams.deviceId = getAudioDeviceId(audioInputName);
        audioInputParams.nChannels = APP_CHANNELS;
        try {
            audioOutput->openStream(&audioOutputParams, &audioInputParams, RTAUDIO_FLOAT32, SAMPLE_RATE, &bufferFrames, &audioCallback);
            audioOutput->startStream();
            // audioInput->openStream(&audioInputParams, NULL, RTAUDIO_FLOAT32, SAMPLE_RATE, &bufferFrames, &audioInputCallback);
            // audioInput->startStream();
            // while (audioOutput->isStreamRunning() && audioInput->isStreamRunning()) {
            while (audioOutput->isStreamRunning()) {
                usleep(100000); // 100ms
            }
        } catch (RtAudioError& e) {
            e.printMessage();
            return 1;
        }
        return 0;
    }

    void list()
    {
        showAudioDeviceInfo();
    }
};

AudioRT* AudioRT::instance = NULL;

#endif