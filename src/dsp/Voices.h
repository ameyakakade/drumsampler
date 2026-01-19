#pragma once
#include <juce_audio_formats/juce_audio_formats.h>

class voice{
    public:
        double playHead;
        bool active;
        int numSamples;

        float playRatio;
        int numChannels;

        int setMidiNote;
        int padID;
        float velocity;

        juce::AudioBuffer<float>* assignedBuffer;

        voice();
        void startVoice(juce::AudioBuffer<float>& buffer, int padNo, int midiNote, float vel, double sRate, double bufferSRate);
        void renderAudio(juce::AudioBuffer<float>& buffer, int startSample, int endSample);
        void quitVoice();
};

struct voiceData{
    std::atomic<bool> state = false;
    std::atomic<int> length = 0;
    std::atomic<int> position = 0;
    std::atomic<int> id = 0;
    voiceData(){
        state = false;
        length = 0;
        position = 0;
        id = 0;
    }
};

class voiceManager{
    private:
        int numVoices;
    public:
        // this vector stores unique pointers to our voices
        std::vector<std::unique_ptr<voice>> voices;
        std::vector<std::unique_ptr<voiceData>> states;
        void prepare(int num);
        void renderAll(juce::AudioBuffer<float>& buffer, int startSample, int endSample);
        void assignVoice(juce::AudioBuffer<float>& buffer, int padNo, int midiNote, float velocity, double sRate, double bufferSRate);

        void updateState(int i, bool state, int length, int pos, int posAdd, int ID);
        void quitByPad(int id);
};

