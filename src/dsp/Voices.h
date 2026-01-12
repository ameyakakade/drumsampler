#pragma once
#include <juce_audio_formats/juce_audio_formats.h>

class voice{
    private:
        double playHead;

    public:
        bool active;

        float playRatio;
        int numSamples;
        int numChannels;

        int setMidiNote;
        float velocity;

        juce::AudioBuffer<float>* assignedBuffer;

        voice();
        void startVoice(juce::AudioBuffer<float>& buffer, int midiNote, float vel, double sRate, double bufferSRate);
        void renderAudio(juce::AudioBuffer<float>& buffer, int startSample, int endSample);
};

class voiceManager{
    private:
        // this vector stores unique pointers to our voices
        std::vector<std::unique_ptr<voice>> voices;
        int numVoices;
    public:
        void prepare(int num);
        void renderAll(juce::AudioBuffer<float>& buffer, int startSample, int endSample);
        void assignVoice(juce::AudioBuffer<float>& buffer, int midiNote, float velocity, double sRate, double bufferSRate);
};
