#pragma once 

#include "../PluginProcessor.h"

class invisibleButton : public juce::Button{
    public:
        invisibleButton();    
        void paintButton (juce::Graphics& g, bool isMouseOver, bool isButtonDown) override;
};

class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor, public juce::Timer, public juce::FileDragAndDropTarget

{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);

    ~AudioPluginAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

    // drag and drop logic
    bool isInterestedInFileDrag(const juce::StringArray &files) override;
    void fileDragEnter(const juce::StringArray &files, int x, int y) override;
    void fileDragMove(const juce::StringArray &files, int x, int y) override;
    void fileDragExit(const juce::StringArray &files) override;
    void filesDropped(const juce::StringArray &files, int x, int y) override;

private:
    AudioPluginAudioProcessor& processorRef;

    juce::Slider gainSlider; 
    juce::Slider pitchSlider; 
    juce::Slider startSlider; 
    juce::Slider endSlider; 
    juce::Slider attackSlider; 
    juce::Slider decaySlider; 
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> pitchAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> startAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> endAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decayAttachment;

    void updateAttachments(int selectedPadIndex);

    int minLength;
    float decayRate;

    int highX;
    int highY;

    std::vector<std::unique_ptr<invisibleButton>> padButtons;

    std::vector<juce::Rectangle<int>> rects;
    std::vector<float> flashes;

    std::atomic<int> selectedPad;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};

