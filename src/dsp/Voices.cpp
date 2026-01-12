#include "Voices.h"
#include <juce_audio_formats/juce_audio_formats.h>

voice::voice(){
    active = false;
    playHead = 0;
}

void voice::startVoice(juce::AudioBuffer<float>& buffer, int midiNote, float vel, double sRate, double bufferSRate){
    active = true;
    numSamples = buffer.getNumSamples();
    numChannels = buffer.getNumChannels();
    playHead = 0;
    assignedBuffer = &buffer;
    setMidiNote = midiNote;
    velocity = vel;
    playRatio = sRate/bufferSRate; 
    DBG("playratio" << playRatio);
}

void voice::renderAudio(juce::AudioBuffer<float>& buffer, int startSample, int endSample){

    int numBufferChannels = buffer.getNumChannels();
    int noOfSamples = endSample - startSample;

    for(int ch = 0; ch<numBufferChannels && ch<numChannels; ch++){
        
        auto* sourceData = assignedBuffer->getReadPointer(ch);
        auto* channelData = buffer.getWritePointer(ch);
        double playHeadNow = playHead;
        

        for(int i = startSample; i<endSample; i++){
            if(playHeadNow+1>=numSamples) break;
            int y = int(playHeadNow);
            float f = playHeadNow - y;
            channelData[i] += sourceData[y]*(1-f) + sourceData[y+1]*f;
            playHeadNow += playRatio;
        }
        
    }

    playHead += noOfSamples*playRatio;

    if(playHead+1>=numSamples){
        playHead = numSamples;
        active = false;
        DBG("Voice stopped:");
        velocity = 0;
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

void voiceManager::renderAll(juce::AudioBuffer<float>& buffer, int startSample, int endSample){
    for(auto& voice : voices){
        if(voice->active){
            voice->renderAudio(buffer, startSample, endSample);
        }
    }
}

void voiceManager::assignVoice(juce::AudioBuffer<float>& buffer, int midiNote, float velocity, double sRate, double bufferSRate){
    for(int i = 0; i<numVoices; i++){
        if(!voices[i]->active){
            voices[i]->startVoice(buffer, midiNote, velocity, sRate, bufferSRate);
            DBG("assigned at"<<i);
            break;
        }
    }
}

