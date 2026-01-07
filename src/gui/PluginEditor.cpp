#include "../PluginProcessor.h"
#include "PluginEditor.h"
// TRANSLATION: "I need the blueprints for both the Brain (Processor) and the Face (Editor)."

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
      // the classname::classname tells the compiler that this function is a constructor of the "AudioPluginAudioProcessorEditor" class
// TRANSLATION: "I am defining the Setup Function (Constructor) for the class AudioPluginAudioProcessorEditor."
// 1. (AudioPluginAudioProcessor& p): "I am accepting the Brain (p) as an input."
// 2. : (The Colon): "BEFORE we start the main code inside { }, do these setup tasks first:"
// 3. AudioProcessorEditor (&p): "Build the Parent Class (the JUCE foundation). It demands the Brain, so pass 'p' to it."
// 4. , processorRef (p): "Save 'p' into my personal variable 'processorRef' so I can use it later." we defined the reference in the .h file
{
    juce::ignoreUnused (processorRef);
    // TRANSLATION: "Hey Compiler, don't yell at me (Warning) for not using 'processorRef' in this function. 
    // I know I haven't touched it yet, just ignore it."

// 1. Configure the Slider
    gainSlider.setSliderStyle (juce::Slider::LinearBarVertical);
    gainSlider.setRange (0.0, 5.0, 0.01); // Min, Max, Step Size
    gainSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 90, 0);
    gainSlider.setValue (0.5);
 
    // 2. Add it to the window (Make it visible)
    addAndMakeVisible (gainSlider);

    // 3. THE CONNECTION (The Magic Wire)
    // "When the slider moves, update the processor's rawVolume variable"
    gainSlider.onValueChange = [this] 
    { 
        processorRef.raw_vol.store(gainSlider.getValue(), std::memory_order_relaxed); 
    };


    delaySlider.setSliderStyle (juce::Slider::LinearBarVertical);
    delaySlider.setRange (0.0, 10.0, 0.01); // Min, Max, Step Size
    delaySlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 90, 0);
    delaySlider.setValue (0.5);
 
    // 2. Add it to the window (Make it visible)
    addAndMakeVisible (delaySlider);

    // 3. THE CONNECTION (The Magic Wire)
    // "When the slider moves, update the processor's rawVolume variable"
    delaySlider.onValueChange = [this] 
    { 
        processorRef.delayTime.store(delaySlider.getValue(), std::memory_order_relaxed); 
    };


    //lets configure the button now
    aButton.setButtonText("i am a button");
    aButton.setToggleable(true);
    aButton.setClickingTogglesState(true);

    aButton.onClick = [this]
    {
        processorRef.distort.store(aButton.getToggleState(), std::memory_order_relaxed);
    };

    //add it to the window
    addAndMakeVisible (aButton);

    pad1Button.setButtonText("Drum 1");
    pad1Button.onClick = [this]
    {
    };
    addAndMakeVisible (pad1Button);


    pad2Button.setButtonText("Drum 2");
    pad2Button.onClick = [this]
    {
    };
    addAndMakeVisible (pad2Button);


    pad3Button.setButtonText("Drum 3");
    pad3Button.onClick = [this]
    {
    };
    addAndMakeVisible (pad3Button);


    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (900, 300);
    // TRANSLATION: "Set the window width to 400 pixels and height to 300 pixels."
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    // TRANSLATION: "The Cleanup Function (Destructor). 
    // It is empty because we haven't created any messy manual memory (pointers) to clean up."
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
// TRANSLATION: "I am defining the paint function belonging to AudioPluginAudioProcessorEditor."
// this g is passed by the os when it calls the paint fn
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    // TRANSLATION: "Artist (g), please paint the ENTIRE window (fillAll) with a specific color."
    // "Look up the standard 'Background Color' from the JUCE theme manager (LookAndFeel)."

    g.setColour (juce::Colours::white);
    // TRANSLATION: "Artist (g), put down the paint roller and pick up a White Pen."


}

void AudioPluginAudioProcessorEditor::resized()
// TRANSLATION: "I am defining the function that runs when the window size changes."
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    gainSlider.setBounds (40, 30, 20, getHeight() - 60);
    delaySlider.setBounds (80, 30, 20, getHeight() - 60);
    aButton.setBounds (140, 30, 50, getHeight()-60);
    pad1Button.setBounds (330, 30, 80, getHeight()-60);
    pad2Button.setBounds (430, 30, 80, getHeight()-60);
    pad3Button.setBounds (530, 30, 80, getHeight()-60);
}
