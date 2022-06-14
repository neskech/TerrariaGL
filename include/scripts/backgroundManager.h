#pragma once
#include "components/component.h"
#include "scene/scene.h"
#include "pch.h"

#define NUM_TEXTURES_DAY = 1
#define NUM_TEXTURES_NIGHT = 1
#define NUM_TEXTURES_UNDERGROUND = 1

enum BackgroundType{
    None = 0,
    Day,
    Night,
    Underground,
};

//Each background element is 3 sprites: One to left of the camera, one in the center, and one to the right
struct Background{
     Terra::Entity* entities[3];
     float parralax;

//      Background(Terra::Entity& left_, Terra::Entity& center_, Terra::Entity& right_, float parralax_):
//                left(left_), center(center_), right(right_), parralax(parralax_)
//      {}

     Background(){}
};

class BackgroundManager : ScriptableObject {
     public:
            BackgroundManager(Terra::Entity& entity_, Scene* scene_) : ScriptableObject(entity_, scene_) {}
           ~BackgroundManager(){}
            void start() override;
            void update(float timeStep) override;
            void onDestroy() override;
            void onCollision(const CollisionData& collision) override;
     private:
            void setAlphaValue(BackgroundType type, float a);
            void setZIndex(BackgroundType type, float z);
            void setPosition(BackgroundType type, float x);
            void presentBackground(BackgroundType type);
            void removeBackground(BackgroundType type);   
            void initDay();
            void initNight();
            void initUnderGround();
            void setBackground(BackgroundType type, Ref<Texture>* textures, float* p, int length);

            std::unordered_map<BackgroundType, std::vector<Background>> backgrounds;
            BackgroundType currentBackground;
            
            float delays[3];
            float elapsedTime = 0.0f;
            bool transitioning = false;
            float transitionAlphaValue = 1.0f;
            BackgroundType transitionBackground;

            float lastTime = 0.0f;
            Biome lastBiome;
            Component::Transform* playerTransform;
            glm::vec2 lastPos;
};