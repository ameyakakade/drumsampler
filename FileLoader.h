#pragma once
#include <juce_audio_formats/juce_audio_formats.h>
#include "FileLoader.h"

void convertFileIntoBuffer(const juce::File& file, juce::AudioBuffer<float>& buffer);

class samplePad{
    private:
        int playPosition;
        juce::AudioBuffer<float> sample;
        bool playState;
        const float* ch0;
        const float* ch1;
        int length;

    public:
        void startPlayback();
        samplePad();
        float playFile(int ch);
        void updateFile(juce::File& file);
};
