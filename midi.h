#ifndef _MIDI_H_
#define _MIDI_H_

#include "audioHandler.h"
#include "def.h"
#include "fs.h"
#include "midiMapping.h"

RtMidiIn midiController;
RtMidiOut midiOut;

MidiMapping midiMappings[] = {
    MidiMapping("GRAIN_START_POSITION", [](float value) {
        // AudioHandler::get().audioGranular.setStart(value);
        // AudioHandler::get().filter.set(value);
    }),
    MidiMapping("GRAIN_DENSIY", [](float value) {
        // AudioHandler::get().audioGranular.setDensity(value);
    }),
    MidiMapping("GRAIN_SIZE", [](float value) {
        // AudioHandler::get().audioGranular.setGrainSize(value);
    }),
    MidiMapping("GRAIN_SPRAY", [](float value) {
        // AudioHandler::get().audioGranular.setSpray(value);
    }),
    MidiMapping("GRAIN_DELAY", [](float value) {
        // AudioHandler::get().audioGranular.setDelay(value);
    }),
    MidiMapping("MASTER_VOLUME", [](float value) {
        setMasterVolume(value);
    }),
    MidiMapping("ENV_ATTACK", [](float value) {
        // AudioHandler::get().audioGranular.setAttack(value);
        AudioHandler::get().sampleRateReducer.sampleStep = value * 100;
        printf("sampleStep %d\n", AudioHandler::get().sampleRateReducer.sampleStep);
    }),
    MidiMapping("ENV_RELEASE", [](float value) {
        // AudioHandler::get().audioGranular.setRelease(value);
    }),
    MidiMapping("SAMPLE_SELECTOR", [](float value) {
        // ...
    }),
    MidiMapping("DISTORTION", [](float value) {
        AudioHandler::get().distortion.set(value);
    }),
    MidiMapping("FILTER_CUTOFF", [](float value) {
        AudioHandler::get().filter.set(value);
    }),
    MidiMapping("FILTER_RESONANCE", [](float value) {
        AudioHandler::get().filter.setResonance(value);
    }),
};

const uint8_t MIDI_MAPS = sizeof(midiMappings) / sizeof(midiMappings[0]);

void midiControllerCallback(double deltatime, std::vector<unsigned char>* message, void* userData)
{
    if (message->at(0) == 0xf8) {
        // ignore midi clock
    } else if (message->at(0) == 0xfe) {
        // ignore active sensing
    // } else if (message->at(0) == 0xe0) {
    //     // Let's forward pitch bend to 4 track of midiOut
    //     // [225,39,100] and [225,39,0]
    //     midiOut.sendMessage(message);
    //     message->at(0) = 0xe1;
    //     midiOut.sendMessage(message);
    //     message->at(0) = 0xe2;
    //     midiOut.sendMessage(message);
    //     message->at(0) = 0xe3;
    //     midiOut.sendMessage(message);

    //     // message->at(0) = 0xe6;
    //     // midiOut.sendMessage(message);
    //     // message->at(0) = 0xe7;
    //     // midiOut.sendMessage(message);
    //     // message->at(0) = 0xe8;
    //     // midiOut.sendMessage(message);

    //     message->at(0) = 0xea;
    //     midiOut.sendMessage(message);
    //     message->at(0) = 0xeb;
    //     midiOut.sendMessage(message);
    //     message->at(0) = 0xec;
    //     midiOut.sendMessage(message);
    //     message->at(0) = 0xed;
    //     midiOut.sendMessage(message);
    } else if (message->at(0) >= 0x90 && message->at(0) < 0xa0) {
        // uint8_t channel = message->at(0) - 0x90;
        // if (channel == midiNodeChannel) {
        // AudioHandler::get().granular.noteOn(message->at(1), message->at(2));
        // AudioHandler::get().pitchShifter.on = message->at(2) > 0;
        // printf("transpose %d\n", AudioHandler::get().pitchShifter.on);
        // }
    } else if (message->at(0) >= 0x80 && message->at(0) < 0x90) {
        // uint8_t channel = message->at(0) - 0x80;
        // if (channel == midiNodeChannel) {
        // AudioHandler::get().granular.noteOff(message->at(1), message->at(2));
        // AudioHandler::get().pitchShifter.on = false;
        // printf("transpose off\n");
        // }
    } else {
        for (int i = 0; i < MIDI_MAPS; i++) {
            if (midiMappings[i].handle(message)) {
                return;
            }
        }
        if (debugMode) {
            debug("Midi controller message: ");
            unsigned int nBytes = message->size();
            for (unsigned int i = 0; i < nBytes; i++) {
                debug("%02x ", (int)message->at(i));
            }
            debug("\n");
        }
    }
}

int getMidiDevice(RtMidi& midi, const char* portName)
{
    unsigned int portCount = midi.getPortCount();

    for (unsigned int i = 0; i < portCount; i++) {
        if (midi.getPortName(i).find(portName) != std::string::npos) {
            return i;
        }
    }
    return -1;
}

bool loadMidiInput(RtMidiIn& midi, const char* portName, RtMidiIn::RtMidiCallback callback)
{
    int port = getMidiDevice(midi, portName);
    if (port == -1) {
        APP_INFO("Midi input %s not found\n", portName);
        return false;
    }

    midi.openPort(port);
    midi.setCallback(callback);
    midi.ignoreTypes(false, false, false);
    APP_INFO("Midi input loaded: %s\n", midi.getPortName(port).c_str());
    return true;
}

bool loadMidiOutput(RtMidiOut& midi, const char* portName)
{
    int port = getMidiDevice(midi, portName);
    if (port == -1) {
        APP_INFO("Midi output %s not found\n", portName);
        return false;
    }

    midi.openPort(port);
    APP_INFO("Midi output loaded: %s\n", midi.getPortName(port).c_str());
    return true;
}

#endif