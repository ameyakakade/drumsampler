#pragma once
#include <juce_audio_formats/juce_audio_formats.h>
#include "../dsp/Voices.h"

double convertFileIntoBuffer(const juce::File& file, juce::AudioBuffer<float>& buffer);

// sample pad holds a audio buffer
class samplePad{
    private:
        juce::AudioBuffer<float> sample;
        static int nextid;

    public:
        int midiNote;
        const int id;
        samplePad();
        juce::AudioBuffer<float>* getFile();
        void updateFile(juce::File& file, juce::AudioFormatManager& formatManager);
        double sampleRate;
};

class samplePadManager{
    private:
        int numOfPads; 
    public:
        std::vector<std::unique_ptr<samplePad>> pads;
        void createPads(int no);
        void updatePadFile(int id, juce::File& inputFile, juce::AudioFormatManager& formatManager);
        std::pair<juce::AudioBuffer<float>*, double> getFileByMidiNote(int note);
};
