#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include "dsp/Effects.h"
#include "data/FileLoader.h"
#include "dsp/Voices.h"
#include <juce_audio_utils/juce_audio_utils.h>


//==============================================================================
class AudioPluginAudioProcessor final : public juce::AudioProcessor
{
public:
    //==============================================================================
    AudioPluginAudioProcessor();
    ~AudioPluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // fn to start playback 
    void startPlayback(int id);

    juce::AudioFormatManager formatManager;
    juce::AudioProcessorValueTreeState states;

    std::atomic<float> raw_vol = {1.0f};
    std::atomic<float> delayTime = {1.0f};
    std::atomic<bool> distort = {false}; 

    samplePadManager samplePool;
    voiceManager pool;

    std::vector<std::unique_ptr<juce::AudioThumbnail>> thumbs;
    juce::AudioThumbnailCache thumbnailCache;
    std::vector<std::unique_ptr<std::atomic<bool>>> padStates;

    void updateFile(juce::String add, int id);

private:
    int playPosition = 0;
    double currentSampleRate;
    int noOfPads;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
};

