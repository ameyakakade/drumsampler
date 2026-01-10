#pragma once
#include <juce_audio_formats/juce_audio_formats.h>

class voice{
    public:
        bool active;

        int playHead;
        int numSamples;
        int numChannels;

        int setMidiNote;
        float velocity;

        juce::AudioBuffer<float>* assignedBuffer;

        voice();
        void startVoice(juce::AudioBuffer<float>& buffer, int midiNote, float vel);
        void renderAudio(juce::AudioBuffer<float>& buffer, int startSample, int noOfSamples);
};

class voiceManager{
    private:
        // this vector stores unique pointers to our voices
        std::vector<std::unique_ptr<voice>> voices;
        int numVoices;
    public:
        void prepare(int num);
        void renderAll(juce::AudioBuffer<float>& buffer, int startSample, int noOfSamples);
        void assignVoice(juce::AudioBuffer<float>& buffer, int midiNote, float velocity);
};
