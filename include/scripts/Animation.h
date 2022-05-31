#pragma once
#include "components/component.h"

class AnimationScript{
     public:
            AnimationScript(Terra::Entity& entity_) : entity(entity_){}
           ~AnimationScript(){}
            void start();
            void update(float timeStep);
            void destroy();
     private:
         Component::AnimationController* aniController;
         Component::SpriteRenderer* sprRend;
         Terra::Entity entity;
};