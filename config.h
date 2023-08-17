#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "def.h"
#include "fs.h"
#include "midi.h"
#include "osc.h"

#define CONFIG_FILE "./config.cfg"

char* trimChar(char* str, char c = '\n')
{
    int len = strlen(str);
    for (int i = 0; i < len; i++) {
        if (str[i] == c) {
            str[i] = '\0';
            break;
        }
    }
    return str;
}

void assignMidiMapping(MidiMapping& map, char* value)
{
    // split value by space
    char* msg0 = strtok(value, " ");
    char* msg1 = strtok(NULL, " ");
    char* msg2 = strtok(NULL, " ");

    if (msg0 == NULL || msg1 == NULL) {
        APP_INFO("Invalid midi mapping\n");
        return;
    }

    uint8_t size = msg2 == NULL ? 2 : 3;
    uint8_t valuePosition = msg1[0] == 'x' && msg1[1] == 'x' ? 2 : 3;
    uint8_t msg0Int = strtol(msg0, NULL, 16);
    uint8_t msg1Int = strtol(msg1, NULL, 16);
    map.set(size, valuePosition, msg0Int, msg1Int);

    APP_INFO("Midi mapping: %02x %02x, size: %d valuePosition: %d\n", msg0Int, msg1Int, size, valuePosition);
}

void assignKeyValue(char* key, char* value)
{
    if (strcmp(key, "MIDIIN") == 0) {
        loadMidiInput(midiController, value, &midiControllerCallback);
    } else if (strcmp(key, "MIDIOUT") == 0) {
        // NOTE note used for the moments...
        loadMidiOutput(midiOut, value);
    } else if (strcmp(key, "AUDIO_OUTPUT") == 0) {
        strcpy(audioOutputName, value);
        APP_INFO("Audio output set: %s\n", audioOutputName);
    } else if (strcmp(key, "AUDIO_INPUT") == 0) {
        strcpy(audioInputName, value);
        APP_INFO("Audio input set: %s\n", audioInputName);
    } else if (strcmp(key, "OSC_SERVER_PORT") == 0) {
        oscServer(value);
    } else if (strcmp(key, "GAIN_OUTPUT") == 0) {
        float gain = AudioHandler::get().gainVolume.setGain(atof(value)).gain;
        APP_INFO("Gain output set: %f\n", gain);
    } else if (strcmp(key, "MIDI_NOTE_CHANNEL") == 0) {
        midiNoteChannel = atoi(value);
        APP_INFO("Midi note channel set: %d\n", midiNoteChannel);
    } else if (strcmp(key, "DEBUG") == 0) {
        if (strcmp(value, "true") == 0) {
            enableDebug();
        }
    } else {
        for (int i = 0; i < MIDI_MAPS; i++) {
            if (strcmp(key, midiMappings[i].key) == 0) {
                assignMidiMapping(midiMappings[i], value);
                return;
            }
        }
        APP_INFO("unknown config key: %s\n", key);
    }
}

void parseConfigLine(char* line)
{
    // ignore comments and empty lines
    if (line[0] == '#' || line[0] == '\n') {
        return;
    }
    // split by '='
    char* key = strtok(line, "=");
    char* value = strtok(NULL, "=");
    if (key == NULL || value == NULL) {
        APP_INFO("Invalid config line: %s\n", line);
        return;
    }
    assignKeyValue(key, trimChar(value));
}

bool loadConfig()
{
    FILE* file = fopen(CONFIG_FILE, "r");
    if (file == NULL) {
        APP_INFO("Failed to load config file: %s\n", CONFIG_FILE);
        return false;
    }

    char line[256];

    while (fgets(line, sizeof(line), file)) {
        parseConfigLine(line);
    }
    fclose(file);

    return true;
}

#endif