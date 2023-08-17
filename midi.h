#ifndef _MIDI_H_
#define _MIDI_H_

#include "audioHandler.h"
#include "def.h"
#include "fs.h"
#include "midiMapping.h"

RtMidiIn midiController;
RtMidiOut midiOut;

MidiMapping midiMappings[] = {
    MidiMapping("MASTER_VOLUME", [](float value) {
        AudioHandler::get().gainVolume.set(value);
    }),
    MidiMapping("SAMPLE_RATE_REDUCER", [](float value) {
        AudioHandler::get().sampleRateReducer.set(value);
    }),
    MidiMapping("GRANULAR_SAMPLE_SELECTOR", [](float value) {
        AudioHandler::get().synthGranular.open(value);
    }),
    MidiMapping("DISTORTION", [](float value) {
        AudioHandler::get().distortion.set(value);
    }),
    MidiMapping("FILTER_CUTOFF", [](float value) {
        AudioHandler::get().filter.setCutoff(value);
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
    } else if (message->at(0) >= 0x90 && message->at(0) < 0xa0) {
        uint8_t channel = message->at(0) - 0x90;
        if (channel == midiNoteChannel) {
            AudioHandler::get().synthGranular.noteOn(message->at(1), message->at(2));
        }
    } else if (message->at(0) >= 0x80 && message->at(0) < 0x90) {
        uint8_t channel = message->at(0) - 0x80;
        if (channel == midiNoteChannel) {
            AudioHandler::get().synthGranular.noteOff(message->at(1), message->at(2));
        }
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