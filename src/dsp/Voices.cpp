#include "Voices.h"
#include <juce_audio_formats/juce_audio_formats.h>


voice::voice(){
    active = false;
    playHead = 0;
    age = 0;
}

void voice::startVoice(juce::AudioBuffer<float>& buffer, int padNo, int midiNote, float vel, double sRate, double bufferSRate){
    active = true;
    numSamples = buffer.getNumSamples();
    numChannels = buffer.getNumChannels();
    playHead = 0;
    assignedBuffer = &buffer;
    setMidiNote = midiNote;
    padID = padNo;
    velocity = vel;
    playRatio = sRate/bufferSRate; 
    age = 0;
}

void voice::renderAudio(juce::AudioBuffer<float>& buffer, int startSample, int endSample, float p){

    int numBufferChannels = buffer.getNumChannels();
    int noOfSamples = endSample - startSample;
    float playRatioNow = playRatio*p;
    age++;

    for(int ch = 0; ch<numBufferChannels && ch<numChannels; ch++){
        
        auto* sourceData = assignedBuffer->getReadPointer(ch);
        auto* channelData = buffer.getWritePointer(ch);
        double playHeadNow = playHead;
        

        for(int i = startSample; i<endSample; i++){
            if(playHeadNow+1>=numSamples) break;
            int y = int(playHeadNow);
            float f = playHeadNow - y;
            channelData[i] += (sourceData[y]*(1-f) + sourceData[y+1]*f)*velocity;
            playHeadNow += playRatioNow;
        }
        
    }

    playHead = playHead + noOfSamples*playRatioNow;

    if(playHead+1>=numSamples){
        playHead = numSamples;
        active = false;
        velocity = 0;
        age = 0;
    }
}

void voice::quitVoice(){
    active = false;
    playHead = 0;
    assignedBuffer = NULL;
    playRatio = 1;
    age = 0;
}

void voiceManager::prepare(int num){ 

    voices.reserve(num);
    states.reserve(num);
    numVoices = num;
    for(int i = 0; i<num; i++){
        voices.emplace_back(std::make_unique<voice>());
        states.emplace_back(std::make_unique<voiceData>());
    }

}

void voiceManager::renderAll(juce::AudioBuffer<float>& buffer, int startSample, int endSample, float p){
    for(int i=0; i<numVoices; i++){
        auto& voice = voices[i];
        updateState(i, voice->active, -1, -1, (endSample-startSample)*(voice->playRatio)*p, -1);
        if(voice->active){
            voice->renderAudio(buffer, startSample, endSample, p);
        }
    }
}

void voiceManager::assignVoice(juce::AudioBuffer<float>& buffer, int padNo, int midiNote, float velocity, double sRate, double bufferSRate){
    int oldest = 0;
    bool assigned = false;
    for(int i = 0; i<numVoices; i++){
        if(voices[oldest]->age < voices[i]->age) oldest = i;
        if(!voices[i]->active){
            assigned = true;
            voices[i]->startVoice(buffer, padNo, midiNote, velocity, sRate, bufferSRate);
            updateState(i, true, buffer.getNumSamples(), 0, -1, padNo);
            break;
        }
    }
    if(!assigned){
        voices[oldest]->startVoice(buffer, padNo, midiNote, velocity, sRate, bufferSRate);
        updateState(oldest, true, buffer.getNumSamples(), 0, -1, padNo);
    }
}

void voiceManager::updateState(int i, bool state, int length, int pos, int posAdd, int ID){
    states[i]->state.store(state, std::memory_order_relaxed);
    if(length!=-1) states[i]->length.store(length, std::memory_order_relaxed);
    if(pos!=-1) states[i]->position.store(pos, std::memory_order_relaxed);
    if(posAdd!=-1){
        int posi = states[i]->position.load(std::memory_order_relaxed);
        states[i]->position.store((posAdd)+posi, std::memory_order_relaxed);
    }
    if(ID!=-1) states[i]->id.store(ID, std::memory_order_relaxed);
}

void voiceManager::quitByPad(int id){
    for(auto& voice : voices) if(voice->padID == id) voice->quitVoice();
}
