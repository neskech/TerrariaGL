#pragma once
#include "components/component.h"

class AnimationScript : Component::ScriptableObject{
     public:
            AnimationScript(Terra::Entity& entity_, Scene* scene_) : ScriptableObject(entity_, scene_) {}
           ~AnimationScript(){}
            void start() override;
            void update(float timeStep) override;
            void onDestroy() override;
            void onCollision(const Component::CollisionData& collision) override;
     private:
         Component::AnimationController* aniController;
         Component::SpriteRenderer* sprRend;
         Component::physicsBody* body;
         Component::Transform* trans;
         Component::Transform* background;
         glm::vec2 lastPos;

};