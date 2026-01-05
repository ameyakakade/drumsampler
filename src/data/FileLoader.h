#pragma once
#include <juce_audio_formats/juce_audio_formats.h>
#include "../dsp/Voices.h"

void convertFileIntoBuffer(const juce::File& file, juce::AudioBuffer<float>& buffer);

// sample pad holds a audio buffer
class samplePad{
    private:
        juce::AudioBuffer<float> sample;
        static int nextid;

    public:
        const int id;
        samplePad();
        juce::AudioBuffer<float>* getFile();
        void updateFile(juce::File& file);
};
