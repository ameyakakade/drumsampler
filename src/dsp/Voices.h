#pragma once
#include <juce_audio_formats/juce_audio_formats.h>

class voice{
    public:

        enum class envelope {Attack, Hold, Decay};

        envelope e;
        double playHead;
        bool active;
        int numSamples;

        float playRatio;
        int numChannels;

        int setMidiNote;
        int padID;
        float velocity;
        int age;
        float oldgain;

        double attackTime;
        double decayTime;
        double attackFactor;
        double decayFactor;
        float startPos;

        juce::AudioBuffer<float>* assignedBuffer;

        voice();
        void startVoice(juce::AudioBuffer<float>& buffer, int padNo, int midiNote, float vel, double sRate, double bufferSRate, float start, float end, float attack, float decay);
        void renderAudio(juce::AudioBuffer<float>& buffer, int startSample, int endSample, float p, float gain);
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
        void renderAll(juce::AudioBuffer<float>& buffer, int startSample, int endSample, float p, std::vector<std::atomic<float>*> g);
        void assignVoice(juce::AudioBuffer<float>& buffer, int padNo, int midiNote, float velocity, double sRate, double bufferSRate, float start, float end, float attack, float decay, bool mono);
        void updateState(int i, bool state, int length, int pos, int posAdd, int ID);
        void quitByPad(int id);
};

