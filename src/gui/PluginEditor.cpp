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
    gain = 0.5;
    gainSlider.setValue (gain);
 
    // 2. Add it to the window (Make it visible)
    addAndMakeVisible (gainSlider);

    // 3. THE CONNECTION (The Magic Wire)
    // "When the slider moves, update the processor's rawVolume variable"
    gainSlider.onValueChange = [this] 
    { 
        processorRef.raw_vol.store(gainSlider.getValue(), std::memory_order_relaxed); 
    };


    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (800, 390);
    // TRANSLATION: "Set the window width to 400 pixels and height to 300 pixels."
    //
    startTimerHz(30);
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
    g.setColour (juce::Colours::cyan);
    // TRANSLATION: "Artist (g), put down the paint roller and pick up a White Pen."

    int x = 80;
    int y = 30;
    for(auto& thumb : processorRef.thumbs){
        double len = thumb->getTotalLength();
        juce::Rectangle<int> rect(x, y ,150, 150);
        thumb->drawChannels(g, rect, 0, len, 1);
        x += 180;
        if(x > 620){
            y += 180; x = 80;
        }
    }
}

void AudioPluginAudioProcessorEditor::resized()
// TRANSLATION: "I am defining the function that runs when the window size changes."
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    gainSlider.setBounds (30, 30, 20, getHeight() - 60);
}


void AudioPluginAudioProcessorEditor::timerCallback(){
    gainSlider.setValue(gain);
    gain = std::fmod(gain+0.1, 5);
    
    for(int t=0; t<30; t++){
        if(processorRef.pool.states[t]->state.load(std::memory_order_relaxed)){
            DBG(processorRef.pool.states[t]->length.load(std::memory_order_relaxed));
            DBG(processorRef.pool.states[t]->position.load(std::memory_order_relaxed));
            DBG(processorRef.pool.states[t]->id.load(std::memory_order_relaxed));
            DBG("-----");
        }
    }

    DBG("*****");
    repaint();
}
