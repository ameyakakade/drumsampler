#include "Voices.h"

void voice::startVoice(juce::AudioBuffer<float>& buffer, int set_id){
    active = true;
    numSamples = buffer.getNumSamples();
    numChannels = buffer.getNumChannels();
    playHead = 0;
}

void voice::renderAudio(juce::AudioBuffer<float>& buffer){
    for(int ch = 0; ch<numChannels; ch++){
        auto* channelData = buffer.getWritePointer(ch);
        for(int i = 0; i<numSamples; i++){
        }
        
    }
}
