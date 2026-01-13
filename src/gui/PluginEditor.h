#pragma once 
// TRANSLATION: "Hey Compiler, read this file only ONE time. 
// If you see it again, ignore it so we don't get stuck in an infinite loop."

#include "../PluginProcessor.h"
// TRANSLATION: "I need to know what a 'Processor' is. 
// Go read the file 'PluginProcessor.h' right now and copy its contents here 
// so I can understand the audio engine."

//==============================================================================
// TRANSLATION: "Just a decorative line to make the code look organized."

class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor, public juce::Timer
// TRANSLATION: 
// 1. class: "I am creating a new Blueprint."
// 2. AudioPluginAudioProcessorEditor: "I am naming this blueprint 'AudioPluginAudioProcessorEditor'."
// 3. final: "This is the final version. No one is allowed to make a sub-version of this blueprint."
// 4. : public: "I am building this ON TOP OF an existing blueprint."
// 5. juce::AudioProcessorEditor: "The base blueprint is the standard Editor from the JUCE library."
// the : means inheritance. we are creating a class that is inherited from AudioProcessorEditor
// the "public" means everyone knows that this class is inherited

{
public:
    // TRANSLATION: "Everything below this line is allowed to be seen and used by the outside world."

    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    // TRANSLATION: "This is the Setup Function (Constructor)."
    // 1. explicit: "You must create me on purpose. You cannot create me by accident/automatically."
    // 2. (AudioPluginAudioProcessor&): "To build me, you MUST hand me the ACTUAL Audio Processor (the '&' symbol). 
    //    Not a copy, not a photo, but the real physical Processor object."

    ~AudioPluginAudioProcessorEditor() override;
    // TRANSLATION: "This is the Cleanup Function (Destructor)."
    // 1. ~: The tilde symbol always means 'Destructor' (Death).
    // 2. override: "I am replacing the standard cleanup instructions with my own custom instructions."

    //==============================================================================
    void paint (juce::Graphics&) override;
    // TRANSLATION: "This is the Drawing Function."
    // 1. void: "I do not return any answer (like a number) when I finish."
    // 2. paint: "My name is paint."
    // 3. (juce::Graphics&): "I require an Artist tool (Graphics object) to do my job."
    // 4. override: "I am replacing the standard 'blank' paint function with my own custom painting."

    void resized() override;
    // TRANSLATION: "This is the Resizing Function."
    // 1. resized: "Run me whenever the user drags the window corner."
    // 2. override: "I am replacing the standard resize behavior."
    
    void timerCallback() override;

private:
    // TRANSLATION: "Everything below this is TOP SECRET. Only code inside this specific file can touch these variables."

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioPluginAudioProcessor& processorRef;
    // TRANSLATION: "This is a permanent link to the Brain."
    // 1. AudioPluginAudioProcessor&: "This variable holds a Reference (link) to the Processor."
    // 2. processorRef: "I am naming this link 'processorRef'."
    // 3. Why? The Editor (Window) needs to talk to the Processor (Audio) to tell it when knobs move.

    juce::Slider gainSlider; 
    double gain;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
    // TRANSLATION: "This is a Magic Macro (a shortcut)."
    // 1. NON_COPYABLE: "Prevent anyone from cloning this window. You cannot have two copies of the same window."
    // 2. LEAK_DETECTOR: "If I forget to delete this window when the plugin closes, scream at the programmer."
};
