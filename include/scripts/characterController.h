#pragma once
#include "components/component.h"

class CharacterController : ScriptableObject{
     public:
            CharacterController(Terra::Entity& entity_, Scene* scene_) : ScriptableObject(entity_, scene_) {}
           ~CharacterController(){}
            void start() override;
            void update(float timeStep) override;
            void onDestroy() override;
            void onCollision(const CollisionData& collision) override;
     private:
       void playerMovementLogic();
       void cameraMovementLogic(float timeStep);
       void handleSettings(float timeStep);

       Component::AnimationController* aniController;
       Component::SpriteRenderer* sprRend;
       Component::physicsBody* body;
       Component::Transform* trans;
       glm::vec2 lastPos;

       bool isJumping = false;

       bool freeCam = false;
       float freeCamSwapElapsedTime = 0.0f;

       bool superSpeed = false;
       float superSpeedEnabledElapsedTime = 0.0f;

       bool gravityEnabled = true;
       float gravityChangeElapsedTime = 0.0f;
};