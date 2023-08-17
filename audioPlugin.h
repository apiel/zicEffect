#ifndef _AUDIO_PLUGIN_H_
#define _AUDIO_PLUGIN_H_

#include "def.h"

class AudioPlugin {
public:
    // TODO add midi mapping: cc, noteon, noteoff...
    // TODO add osc mapping
    // TODO add config, e.g.: gainOutput

    virtual float sample(float in) = 0;
};

#endif