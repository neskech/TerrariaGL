#pragma once
#include <stdlib.h>

struct Animation{
    uint16_t row;
    uint16_t colStart, colEnd;
    float frameDeltaTime, elapsedTime;
    uint16_t currentFrame;

    Animation(){}
    
    Animation(uint16_t row_, uint16_t colStart_, uint16_t frameLength, float frameDeltaTime_)
    : row(row_), colStart(colStart_), colEnd(colStart_ + frameLength - 1) , frameDeltaTime(frameDeltaTime_), currentFrame(0)
    {

    }

    void advance(float timeStep){
        elapsedTime += timeStep;
        if (elapsedTime >= frameDeltaTime){
            elapsedTime = 0.0f;
            currentFrame = (currentFrame + 1) % (colEnd - colStart);
        } 
    }
};


