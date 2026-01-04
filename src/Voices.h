#pragma once
#include <juce_audio_formats/juce_audio_formats.h>

class voice{
    public:
        voice();
        bool active;
        
        int playHead;
        float numSamples;
        float numChannels;

        // to keep track of voices for monophony
        const int voiceID;
        static int nextID;

        juce::AudioBuffer<float>& assignedBuffer;

        void startVoice(juce::AudioBuffer<float>& buffer, int set_id);
        void renderAudio(juce::AudioBuffer<float>& buffer);
};
