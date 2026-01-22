#include "FileLoader.h"
#include "../dsp/Voices.h"

// passing in the reference to the buffer in which file is to be loaded into
double convertFileIntoBuffer(const juce::File& file, juce::AudioBuffer<float>& buffer, juce::AudioFormatManager& formatManager){

    auto* reader = formatManager.createReaderFor(file);
    // this pointer will be null if file not found

    if(reader!=nullptr){

        //DBG("the pointer for file reader is not null");
        // we set the size for our audio buffer by giving it the number of samples
        // and channels in our audio reader
        buffer.setSize(reader->numChannels, reader->lengthInSamples);

        // we pass in the address of the audio buffer which will be filled by the contents of the file.
        reader->read(&buffer, 0, reader->lengthInSamples, 0, true, true);
        // (address of the buffer, start at 0 of buffer, no of samples, start at 0 of reader, some stereo shit)

        double sRate = reader->sampleRate;

        // we do not need the reader now so just delete it.
        // data is stored in the audio buffer
        delete reader; 

        return sRate;
    }
}


void samplePad::updateFile(juce::File& file, juce::AudioFormatManager& formatManager){
    filePath = file.getFullPathName();
    sampleRate = convertFileIntoBuffer(file, sample, formatManager);
}

// this will return a pointer to the audio buffer
juce::AudioBuffer<float>* samplePad::getFile(){
    return &sample;
}

void samplePadManager::createPads(int no){ 
    numOfPads = no;
    for(int i = 0; i<no; i++){
        pads.emplace_back(std::make_unique<samplePad>());
        pads[i]->id = i;
    }
    for(int i = 0; i<no; i++){
        //DBG(pads[i]->id);
    }
    int start = 60;
    for(auto& pad : pads){
        pad->midiNote = start;
        start++;
    }
}


juce::File& samplePadManager::updatePadFile(int id, juce::File& inputFile, juce::AudioFormatManager& formatManager){
    pads[id]->updateFile(inputFile, formatManager);
    return inputFile;
}


std::unique_ptr<padData> samplePadManager::getFileByMidiNote(int note){
    std::unique_ptr<padData> res = std::make_unique<padData>();
    for(auto& pad : pads){
        if(pad->midiNote == note){
            res->file = pad->getFile();
            res->sampleRate = pad->sampleRate;
            res->id = pad->id;
            return res;
        }
    }
    return res;
}
