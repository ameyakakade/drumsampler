#pragma once
#include <queue>
#include <juce_audio_processors/juce_audio_processors.h>
void distortFn (juce::AudioBuffer<float>& buffer, float d);

class delayBuffer{

private:
    std::queue<float> delayBLeft;
    std::queue<float> delayBRight;
public:
    delayBuffer();
    void apply(juce::AudioBuffer<float>& buffer, float sizei, float feedback);
};
