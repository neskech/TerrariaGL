#pragma once
#include "components/component.h"
#include "scene/scene.h"
#include "pch.h"

enum BackgroundType{
    None = 0,
    forest,
    tundra,
    desert,
    underground,
};

//Each background element is 3 sprites: One to left of the camera, one in the center, and one to the right
struct Background{
     Terra::Entity* entities[3];
     float parralax;

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
            void setZIndex(BackgroundType type, int z);
            void setPosition(BackgroundType type, const glm::vec2& pos);
            void presentBackground(BackgroundType type);
            void removeBackground(BackgroundType type);   

            void initForest();
            void initSnow();
            void initDesert();
            void initUnderGround();
            BackgroundType biomeToBackground(Biome b);
            void setBackground(BackgroundType type, Ref<Texture>* textures, float* p, int length);

            void updateSunAndMoon();

            std::unordered_map<BackgroundType, std::vector<Background>> backgrounds;
            BackgroundType currentBackground;
            
            bool transitioning = false;
            float transitionAlphaValue = 1.0f;
            BackgroundType transitionBackground;

            Biome lastBiome;
            glm::vec2 lastCameraPos;

            Terra::Entity* sunAndMoon;
            bool day = true;
            float elapsedTime = 0.0f;
            float sunAndMoonTheta = 0.0f;
};