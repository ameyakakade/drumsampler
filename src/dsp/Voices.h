#pragma once
#include <juce_audio_formats/juce_audio_formats.h>

class voice{
    public:
        bool active;
        
        int playHead;
        float numSamples;
        float numChannels;

        // to keep track of voices for monophony
        //const int voiceID;
        //static int nextID;

        juce::AudioBuffer<float>* assignedBuffer;

        voice();
        void startVoice(juce::AudioBuffer<float>& buffer, int set_id);
        void renderAudio(juce::AudioBuffer<float>& buffer);
};

class voiceManager{
    private:
        // this vector stores unique pointers to our voices
        std::vector<std::unique_ptr<voice>> voices;
        int numVoices;
    public:
        void prepare(int num);
        void renderAll(juce::AudioBuffer<float>& buffer);
        void assignVoice(juce::AudioBuffer<float>& buffer);
};
