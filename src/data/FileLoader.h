#pragma once
#include <juce_audio_formats/juce_audio_formats.h>
#include "../dsp/Voices.h"

double convertFileIntoBuffer(const juce::File& file, juce::AudioBuffer<float>& buffer);

struct padData{
    juce::AudioBuffer<float>* file;
    double sampleRate;
    int id;
    padData(){
        file = nullptr;
        sampleRate = 0;
        id = 0;
    }
    padData(juce::AudioBuffer<float>* f, double sRate, int padid){
        file = f;
        sampleRate = sRate;
        id = padid;
    }
};

// sample pad holds a audio buffer
class samplePad{
    private:
        juce::AudioBuffer<float> sample;

    public:
        int midiNote;
        int id;
        juce::AudioBuffer<float>* getFile();
        void updateFile(juce::File& file, juce::AudioFormatManager& formatManager);
        double sampleRate;
        juce::String filePath;
};

class samplePadManager{
    private:
        int numOfPads; 
    public:
        std::vector<std::unique_ptr<samplePad>> pads;
        void createPads(int no);
        juce::File& updatePadFile(int id, juce::File& inputFile, juce::AudioFormatManager& formatManager);
        std::unique_ptr<padData> getFileByMidiNote(int note);
};
