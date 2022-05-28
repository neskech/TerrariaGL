#pragma once

struct Animation{
    int row;
    int colStart, colEnd;
    float dTime;
    float elapsedTime;
    int currentFrame;

    //set Spritesheets current sprite according to this each frame
    void advance(float dt){
        elapsedTime += dt;
        if (elapsedTime >= dTime){
            elapsedTime = 0.0f;
            currentFrame = (currentFrame + 1) % (colEnd - colStart);
        } 
    }
};

template <const int numAnimations>
class Animator{
    public:
        Animator();
        ~Animator();

        void addAnimation()
    private:
        Animation animations[numAnimations];
};