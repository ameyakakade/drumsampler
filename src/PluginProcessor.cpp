#include "PluginProcessor.h"
#include "gui/PluginEditor.h"
#include "dsp/Effects.h"
#include "data/FileLoader.h"
#include <queue>
#include <algorithm>



//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
        // TRANSLATION: "I am initializing the Parent (AudioProcessor).
        // I am handing it a list of 'BusesProperties' (Inputs/Outputs)."

                     #if ! JucePlugin_IsMidiEffect
                       // TRANSLATION: "If I am NOT a MIDI effect (like a synth or pure MIDI tool)..."
                      #if ! JucePlugin_IsSynth
                       // TRANSLATION: "...and if I am NOT a Synthesizer..."
                        .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                        // TRANSLATION: "...then please give me a Stereo Input pair."
                      #endif
                        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                        // TRANSLATION: "Also, give me a Stereo Output pair."
                     #endif
                       ),
     thumbnailCache(5)
#endif
{

    formatManager.registerBasicFormats();

    int no = 8;
    int noSFT = 32;

    samplePool.createPads(no);

    for(int i=0; i<no; i++){
        thumbs.emplace_back(std::make_unique<juce::AudioThumbnail>(noSFT, formatManager, thumbnailCache));
        padStates.emplace_back(std::make_unique<std::atomic<bool>>());
    }

    updateFile("~/Documents/Music/demo/kick.wav", 0);
    updateFile("~/Documents/Music/demo/snare.wav", 1);
    updateFile("~/Documents/Music/demo/hat.wav", 2);
    updateFile("~/Documents/Music/demo/openhat.wav", 3);
    updateFile("~/Documents/Music/demo/808.wav", 4);
    updateFile("~/Documents/Music/demo/stomp.wav", 5);
    updateFile("~/Documents/Music/demo/fx.wav", 6);
    updateFile("~/Documents/Music/demo/clap.wav", 7);

    pool.prepare(30);
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
    // TRANSLATION: "Destructor. Nothing to clean up here yet."
}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const
{
    // TRANSLATION: "My name is defined in the CMake settings (e.g., 'MyDelayPlugin')."
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
    return true;
}

bool AudioPluginAudioProcessor::producesMidi() const
{
    return false;
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
    return false;
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    // TRANSLATION: "When the user hits Stop, silence me immediately (0.0s). 
    // (If this were a Reverb, we might return 5.0s so the tail rings out)."
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    // TRANSLATION: "I have 1 program (the current one). 
    // (DAWs get confused if you say 0, so we lie and say 1)."
    return 1;
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String AudioPluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    // TRANSLATION: "The User just hit Play (or loaded the plugin)."
    // "This is where I reset my math. If I had a Delay line, I would clear the memory here."
}

void AudioPluginAudioProcessor::releaseResources()
{
    // TRANSLATION: "The User just deleted the plugin or closed the session."
    // "Free up any heavy memory I reserved."
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // TRANSLATION: "This checks if the DAW is trying to force a channel layout we don't support."
    // "We only support Mono or Stereo."
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // "We also require Input and Output to match (Stereo In -> Stereo Out)."
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif
;
    return true;
  #endif
}

void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    // TRANSLATION: This is the AUDIO LOOP. It runs ~44,000 times a second.

    // checks for clicks from ui 
    // fn that checks if a trigger is on and adds it to the voice pool    

    juce::ScopedNoDenormals noDenormals; // CPU safety feature.
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    // TRANSLATION: Safety Cleanup.
    // "If we have 2 Inputs but 4 Outputs, silence the extra 2 Outputs so they don't scream noise."


    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    int noOfSamples = buffer.getNumSamples();

    float delay_time = delayTime.load(std::memory_order_relaxed);
    float d = raw_vol.load(std::memory_order_relaxed);
    bool check = distort.load(std::memory_order_relaxed);
    float sizeOfQueue = delayTime*currentSampleRate;
    
    int head = 0;
    
    for(const auto metadata : midiMessages){
        auto msg = metadata.getMessage();
        int time = metadata.samplePosition;
        if(msg.isNoteOn()){
            int note = msg.getNoteNumber();    
            DBG(note);
            float v = msg.getFloatVelocity();
            auto data = samplePool.getFileByMidiNote(note);
            if(data->file){
                pool.assignVoice(*data->file, data->id, note, v, data->sampleRate, currentSampleRate);
            }else{
            }
            padStates[data->id]->store(true, std::memory_order_relaxed);
        }
        if(msg.isNoteOff()){
        }
        pool.renderAll(buffer, head, time);
        head = time;
    }
    pool.renderAll(buffer, head, noOfSamples);

}

void AudioPluginAudioProcessor::updateFile(std::string add, int id){
    juce::File file(add);
    pool.quitByPad(id);
    samplePool.updatePadFile(id, file, formatManager);
    thumbs[id]->setSource(new juce::FileInputSource(file));

}


//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    // TRANSLATION: "Yes, I have a custom GUI window."
    return true; 
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    // TRANSLATION: "Create the Editor Window."
    // "Pass '*this' (Me, the Processor) to the Editor so it can control me."
    return new AudioPluginAudioProcessorEditor (*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // TRANSLATION: "SAVE STATE. The user hit Ctrl+S."
    // "Convert all knob positions into binary data and store it in destData."
    juce::ignoreUnused (destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // TRANSLATION: "LOAD STATE. The user loaded a project."
    // "Read the binary data and move the knobs back to where they were."
    juce::ignoreUnused (data, sizeInBytes);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    // TRANSLATION: "Hello Reaper. You asked for a plugin?"
    // "Here is a new instance of my Processor."
    return new AudioPluginAudioProcessor();
}

