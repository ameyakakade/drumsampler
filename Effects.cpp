#include "Effects.h"
#include <cmath>
#include <queue>

void distortFn (float* buffer, int size, float d){
    for(int i = 0; i<size; i++){
        buffer[i] = std::tanh(d*buffer[i]);
    }
}

delayBuffer::delayBuffer(){
    delayB = {};
}
void delayBuffer::update(float sample){
    delayB.push(sample);
} 
float delayBuffer::get(float sizei){
    float res;
    if(delayB.size()>sizei){
        delayB.pop();
    }
    if(delayB.size()>=sizei){
        res = delayB.front();
        delayB.pop();
    }else{
        res = 0;
    }
    return res;
} 
