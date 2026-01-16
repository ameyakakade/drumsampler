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

    minLength = 1024*4; //minimum length for playhead to be drawn

    int x = 80;
    int y = 30;
    for(int i=0; i<8; i++){
        juce::Rectangle<int> rect(x, y ,150, 150);
        rects.push_back(rect);
        x += 180;
        if(x > 620){
            y += 180; x = 80;
        }
    }

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
    g.fillAll (juce::Colours::black);
    // TRANSLATION: "Artist (g), please paint the ENTIRE window (fillAll) with a specific color."
    // "Look up the standard 'Background Color' from the JUCE theme manager (LookAndFeel)."
    g.setColour (juce::Colours::cyan);
    // TRANSLATION: "Artist (g), put down the paint roller and pick up a White Pen."

    for(int i=0; i<8; i++){
        auto& thumb = processorRef.thumbs[i];
        juce::Rectangle<int> rect = rects[i];
        thumb->drawChannels(g, rect, 0, thumb->getTotalLength(), 1);
    }

    g.setColour (juce::Colours::red);

    for(int t=0; t<30; t++){
        if(processorRef.pool.states[t]->state.load(std::memory_order_relaxed)){
            int len = processorRef.pool.states[t]->length.load(std::memory_order_relaxed);
            int posi = processorRef.pool.states[t]->position.load(std::memory_order_relaxed);
            int id = processorRef.pool.states[t]->id.load(std::memory_order_relaxed);
            if(len>minLength){
                float offset = (float)posi/(float)len*150.0f;
                DBG("offset"<<offset<<len<<posi);
                int x = rects[id].getX();
                int y = rects[id].getY();
                g.drawLine(x + offset, y, x + offset, y + 150, 3);
            }
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
    repaint();
}
