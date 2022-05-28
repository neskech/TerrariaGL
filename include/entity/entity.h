#pragma once

#include "entity/spriteRenderer.h"
#include "entity/animator.h"

class Entity{
    public:
        Entity();
        virtual ~Entity();

        virtual void update();

        SpriteRenderer& getSpriteRenderer(){ return renderer; }
        Animator& getAnimator(){ return animator; }

        Transform transform;
    protected:
        SpriteRenderer renderer;
        Animator animator;
}