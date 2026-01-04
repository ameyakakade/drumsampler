#include <juce_audio_formats/juce_audio_formats.h>
#include "SamplePad.h"

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
    playPosition = 0;
    playState = false;
    ch0 = NULL;
    ch1 = NULL;
}

void samplePad::updateFile(juce::File& file){
    convertFileIntoBuffer(file, sample);
    length = sample.getNumSamples();
    ch0 = sample.getReadPointer(0);
    ch1 = sample.getReadPointer(1);
}

void samplePad::startPlayback(){
    if (ch0==NULL | ch1==NULL) {return;}
    playPosition = 0;
    playState = true;
    DBG("ID" << id);
}

float samplePad::playFile(int ch){
    // return zero if playstate is false
    if(playState==false){return 0;}

    float singleSample = 0;
    if(ch==0){
        singleSample = ch0[playPosition];
    }else if(ch==1){
        singleSample = ch1[playPosition];
        playPosition++;
    }

    if(playPosition>=length){playState = false;}
    return singleSample;
}
