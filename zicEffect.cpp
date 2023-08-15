#include "audioApi.h"
#include "config.h"
#include "def.h"

#define AUDIO_API_PULSE 1
#define AUDIO_API_ALSA 2

#ifndef AUDIO_API
// #define AUDIO_API AUDIO_API_ALSA
#endif

#if AUDIO_API == AUDIO_API_PULSE
#include "audioApi_pulse.h"
AudioApi& audioApi = AudioApiPulse::get();
#elif AUDIO_API == AUDIO_API_ALSA
#include "audioApi_alsa.h"
AudioApi& audioApi = AudioApiAlsa::get();
#else
#include "audioApi_rt.h"
AudioApi& audioApi = AudioApiRT::get();
#endif

int main(int argc, char* args[])
{
    if (argc > 1) {
        if (strcmp(args[1], "-h") == 0 || strcmp(args[1], "--help") == 0) {
            APP_PRINT("Usage:\n    %s\n    %s --list\n", args[0], args[0]);
            return 0;
        }

        if (strcmp(args[1], "--list") == 0) {
            // showAudioDeviceInfo();
            audioApi.list();
            return 0;
        }
    }

    // TODO make config.cfg a parameter
    // make default config
    if (loadConfig() == false) {
        return 1;
    }

    return audioApi.open();
}