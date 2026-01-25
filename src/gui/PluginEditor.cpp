#include "../PluginProcessor.h"
#include "PluginEditor.h"


invisibleButton::invisibleButton() : juce::Button("invisible_button"){

}


void invisibleButton::paintButton (juce::Graphics& g, bool isMouseOver, bool isButtonDown){
    g.setColour (juce::Colours::red);
    g.setOpacity(0);
    juce::Rectangle<int> rect(0, 0 ,150, 150);
    g.fillRect(rect);
}


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
        padButtons.emplace_back(std::make_unique<invisibleButton> ());
    }

    updateAttachments(0);

    for(int i=0; i<8; i++){
        addAndMakeVisible(*padButtons[i]);
        padButtons[i]->onClick = [this, i]{updateAttachments(i);};
    }



    gainSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    gainSlider.setRange (0.0, 1.0, 0.01); // Min, Max, Step Size
    gainSlider.setTextBoxStyle (juce::Slider::NoTextBox, true, 120, 120);
 
    pitchSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    pitchSlider.setRange (0.0, 1.0, 0.01); // Min, Max, Step Size
    pitchSlider.setTextBoxStyle (juce::Slider::NoTextBox, true, 120, 120);

    startSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    startSlider.setRange (0.0, 1.0, 0.01); // Min, Max, Step Size
    startSlider.setTextBoxStyle (juce::Slider::NoTextBox, true, 120, 120);

    endSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    endSlider.setRange (0.0, 1.0, 0.01); // Min, Max, Step Size
    endSlider.setTextBoxStyle (juce::Slider::NoTextBox, true, 120, 120);

    attackSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    attackSlider.setRange (0.0, 1.0, 0.01); // Min, Max, Step Size
    attackSlider.setTextBoxStyle (juce::Slider::NoTextBox, true, 120, 120);

    decaySlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    decaySlider.setRange (0.0, 1.0, 0.01); // Min, Max, Step Size
    decaySlider.setTextBoxStyle (juce::Slider::NoTextBox, true, 120, 120);

    addAndMakeVisible (gainSlider);
    addAndMakeVisible (pitchSlider);
    addAndMakeVisible (startSlider);
    addAndMakeVisible (endSlider);
    addAndMakeVisible (attackSlider);
    addAndMakeVisible (decaySlider);

    setSize (800, 390);
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
        g.setColour (juce::Colours::cyan);
        auto& thumb = processorRef.thumbs[i];
        juce::Rectangle<int> rect = rects[i];
        thumb->drawChannels(g, rect, 0, thumb->getTotalLength(), 1);
        g.setColour (juce::Colours::black);
        g.setOpacity(0.85f);
        float s = processorRef.start[i]->load(std::memory_order_relaxed);
        float e = processorRef.end[i]->load(std::memory_order_relaxed);
        g.fillRect(rect.getX(), rect.getY(), int(rect.getWidth()*s), rect.getHeight());
        g.fillRect(int(rect.getX()+(rect.getWidth()*e)), rect.getY(), int(rect.getWidth()*(1-e)), rect.getHeight());
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
                    float x = rects[id].getX();
                    float y = rects[id].getY();
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
    
    g.setColour (juce::Colours::pink);
    g.drawRect(rects[selectedPad.load(std::memory_order_relaxed)]);

    g.setOpacity(1);
}

void AudioPluginAudioProcessorEditor::resized()
// TRANSLATION: "I am defining the function that runs when the window size changes."
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    gainSlider.setBounds (10, 10, 60, 60);
    pitchSlider.setBounds (10, 70, 60, 60);
    startSlider.setBounds (10, 130, 60, 60);
    endSlider.setBounds (10, 190, 60, 60);
    attackSlider.setBounds (10, 250, 60, 60);
    decaySlider.setBounds (10, 310, 60, 60);
    for(int i=0; i<8; i++){
        padButtons[i]->setBounds(rects[i]);
    }
}


void AudioPluginAudioProcessorEditor::timerCallback(){
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


void AudioPluginAudioProcessorEditor::updateAttachments(int selectedPadIndex){
    
    gainAttachment.reset();
    pitchAttachment.reset();
    startAttachment.reset();
    endAttachment.reset();
    attackAttachment.reset();
    decayAttachment.reset();

    selectedPad.store(selectedPadIndex);
    int i = selectedPad.load();

    gainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.states, "Gain"+std::to_string(i), gainSlider); 
    pitchAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.states, "Pitch"+std::to_string(i), pitchSlider); 
    startAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.states, "Start"+std::to_string(i), startSlider); 
    endAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.states, "End"+std::to_string(i), endSlider); 
    attackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.states, "Attack"+std::to_string(i), attackSlider); 
    decayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.states, "Decay"+std::to_string(i), decaySlider); 
}
