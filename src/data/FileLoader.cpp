#include <juce_audio_formats/juce_audio_formats.h>
#include "FileLoader.h"
#include "../dsp/Voices.h"

// passing in the reference to the buffer in which file is to be loaded into
void convertFileIntoBuffer(const juce::File& file, juce::AudioBuffer<float>& buffer){
    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();
    auto* reader = formatManager.createReaderFor(file);
    // this pointer will be null if file not found

    if(reader!=nullptr){

        DBG("the pointer for file reader is not null");
        // we set the size for our audio buffer by giving it the number of samples
        // and channels in our audio reader
        buffer.setSize(reader->numChannels, reader->lengthInSamples);

        // we pass in the address of the audio buffer which will be filled by the contents of the file.
        reader->read(&buffer, 0, reader->lengthInSamples, 0, true, true);
        // (address of the buffer, start at 0 of buffer, no of samples, start at 0 of reader, some stereo shit)


        // we do not need the reader now so just delete it.
        // data is stored in the audio buffer
        delete reader; 
    }
}

int samplePad::nextid = 1;

samplePad::samplePad() : id(nextid++){
}

void samplePad::updateFile(juce::File& file){
    convertFileIntoBuffer(file, sample);
}

// this will return a pointer to the audio buffer
juce::AudioBuffer<float>* samplePad::getFile(){
    return &sample;
}
