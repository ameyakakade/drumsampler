#include "../PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);

    minLength = 1024*4; //minimum length for playhead to be drawn
    decayRate = 0.7f;
    flashes.resize(8);

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
    startTimerHz(60);
    
    highX = highY = -1;

}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{

}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);

    g.setColour (juce::Colours::cyan);


    for(int i=0; i<8; i++){
        auto& thumb = processorRef.thumbs[i];
        juce::Rectangle<int> rect = rects[i];
        thumb->drawChannels(g, rect, 0, thumb->getTotalLength(), 1);
    }


    for(int t=0; t<30; t++){
        if(processorRef.pool.states[t]->state.load(std::memory_order_relaxed)){
            int len = processorRef.pool.states[t]->length.load(std::memory_order_relaxed);
            int posi = processorRef.pool.states[t]->position.load(std::memory_order_relaxed);
            int id = processorRef.pool.states[t]->id.load(std::memory_order_relaxed);
            g.setColour (juce::Colours::red);
            if(len>minLength){
                float offset = (float)posi/(float)len*150.0f;
                if(offset<150){
                    g.setOpacity(1-(offset/300.0));
                    int x = rects[id].getX();
                    int y = rects[id].getY();
                    g.drawLine(x + offset, y, x + offset, y + 150, 2);
                } 
            }
        }
        g.setOpacity(1);
    }

    for(int i=0; i<flashes.size(); i++){
        bool flag = processorRef.padStates[i]->load(std::memory_order_relaxed);
        if(flag){
            flashes[i] = 1.0f;
            processorRef.padStates[i]->store(false, std::memory_order_relaxed);
        }
        else{
            g.setColour (juce::Colours::cyan);
            g.setOpacity(flashes[i]);
            g.fillRect(rects[i]);
            flashes[i] *= decayRate;
        }
    }

    if(highX!=-1 or highY!=-1){
        g.setColour (juce::Colours::red);
        g.setOpacity(0.8);
        int r = 40;
        g.fillEllipse(highX-(r/2), highY-(r/2), r, r);
    }

    g.setOpacity(1);
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




bool AudioPluginAudioProcessorEditor::isInterestedInFileDrag(const juce::StringArray &files){
    return true;
}

void AudioPluginAudioProcessorEditor::fileDragEnter(const juce::StringArray &files, int x, int y){
}

void AudioPluginAudioProcessorEditor::fileDragMove(const juce::StringArray &files, int x, int y){
    highX = x;
    highY = y;
}

void AudioPluginAudioProcessorEditor::fileDragExit(const juce::StringArray &files){
    highX = -1;
    highY = -1;
}

void AudioPluginAudioProcessorEditor::filesDropped(const juce::StringArray &files, int x, int y){
    highX = -1;
    highY = -1;
    int id = -1;
    int s = files.size();
    for(int i=0; i<8; i++){
        auto& rect = rects[i];
        int rx = rect.getX();
        int ry = rect.getY();
        int h = rect.getHeight();
        int w = rect.getWidth();
        if((x-rx<w) and (y-ry<h) and (x-rx>0) and (y-ry>0)) id = i;
    }
    if(id!=-1){
        for(int i=0; i<s; i++){
            std::string path = files[i].toStdString();
            processorRef.updateFile(path, id);
            id++;
            if(id>=8) break;
        }
    }
}
