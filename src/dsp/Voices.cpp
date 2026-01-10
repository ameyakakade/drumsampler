#include "Voices.h"
#include <juce_audio_formats/juce_audio_formats.h>

voice::voice(){
    active = false;
    playHead = 0;
}

void voice::startVoice(juce::AudioBuffer<float>& buffer, int midiNote, float vel){
    active = true;
    numSamples = buffer.getNumSamples();
    numChannels = buffer.getNumChannels();
    playHead = 0;
    assignedBuffer = &buffer;
    setMidiNote = midiNote;
    velocity = vel;
}

void voice::renderAudio(juce::AudioBuffer<float>& buffer, int startSample, int noOfSamples){

    int numBufferChannels = buffer.getNumChannels();

    for(int ch = 0; ch<numBufferChannels && ch<numChannels; ch++){
        
        auto* sourceData = assignedBuffer->getReadPointer(ch);
        auto* channelData = buffer.getWritePointer(ch);
        int playHeadNow = playHead;

        for(int i = startSample; i<startSample+noOfSamples; i++){
            if(playHeadNow>=numSamples) break;
            channelData[i] += sourceData[playHeadNow++]*velocity;
        }
        
    }

    playHead += noOfSamples;

    if(playHead>=numSamples){
        playHead = numSamples;
        active = false;
        DBG("Voice stopped:");
    }
}



void voiceManager::prepare(int num){ 

    voices.reserve(num);
    numVoices = num;
    for(int i = 0; i<num; i++){
        voices.emplace_back(std::make_unique<voice>());
    }
    //DBG("added voices:"<<num);

}

void voiceManager::renderAll(juce::AudioBuffer<float>& buffer, int startSample, int noOfSamples){
    for(auto& voice : voices){
        if(voice->active){
            voice->renderAudio(buffer, startSample, noOfSamples);
        }
    }
}

void voiceManager::assignVoice(juce::AudioBuffer<float>& buffer, int midiNote, float velocity){
    for(int i = 0; i<numVoices; i++){
        if(!voices[i]->active){
            voices[i]->startVoice(buffer, midiNote, velocity);
            DBG("assigned at"<<i);
            break;
        }
    }
}

