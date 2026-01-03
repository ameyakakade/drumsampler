#pragma once
#include <queue>
void distortFn (float* buffer, int size, float d);

class delayBuffer{

private:
    std::queue<float> delayB;
public:
    delayBuffer();
    void update(float sample);
    float get(float sizei);
};
