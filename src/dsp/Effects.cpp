#include "Effects.h"
#include <cmath>
#include <queue>

void distortFn (juce::AudioBuffer<float>& buffer, float d){
    auto numSamples = buffer.getNumSamples();
    auto numChannels = buffer.getNumChannels();
    for (int ch=0; ch<numChannels; ch++){
        auto* channelData = buffer.getWritePointer(ch);
        for(int i=0; i<numSamples; i++){
            channelData[i] = std::tanh(d*channelData[i]);
        }
    }
}

delayBuffer::delayBuffer(){
    delayBLeft = {};
    delayBRight = {};
}

void delayBuffer::apply(juce::AudioBuffer<float>& buffer, float sizei, float feedback){
    auto left = buffer.getWritePointer(0);
    auto right = buffer.getWritePointer(1);
    auto numSamples = buffer.getNumSamples();
    feedback /= 5; 
    for(int i = 0; i<numSamples; i++){
        if(delayBLeft.size()>sizei || delayBRight.size()>sizei){
            delayBLeft.pop();
            delayBRight.pop();
        }
        if(delayBLeft.size()>=sizei || delayBRight.size()>=sizei){
            left[i] += delayBLeft.front()*feedback;
            right[i] += delayBRight.front()*feedback;
            delayBLeft.pop();
            delayBRight.pop();
        }

        delayBLeft.push(left[i]);
        delayBRight.push(right[i]);

    }
} 
