#include "Voices.h"

void voice::startVoice(juce::AudioBuffer<float>& buffer, int set_id){
    active = true;
    numSamples = buffer.getNumSamples();
    numChannels = buffer.getNumChannels();
    playHead = 0;
    assignedBuffer = buffer;
}

void voice::renderAudio(juce::AudioBuffer<float>& buffer){

    float numBufferSamples = buffer.getNumSamples();
    float numBufferChannels = buffer.getNumChannels();

    for(int ch = 0; ch<numBufferChannels && ch<numChannels; ch++){
        
        auto* sourceData = assignedBuffer.getReadPointer(ch);
        auto* channelData = buffer.getWritePointer(ch);
        int playHeadNow = playHead;

        for(int i = 0; i<numBufferSamples; i++){
            if(playHeadNow>=numSamples) break;
            channelData[i] += sourceData[playHeadNow++];
        }
        
    }

    playHead += numBufferSamples;

    if(playHead>=numSamples){
        playHead = numSamples;
        active = false;
    }
}
