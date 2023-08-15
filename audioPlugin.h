#ifndef _AUDIO_PLUGIN_H_
#define _AUDIO_PLUGIN_H_

#include "def.h"

class AudioPlugin {
public:
    // might also want audio mapping
    
    virtual float sample(float in) = 0;
};

#endif