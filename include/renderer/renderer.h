#pragma once
#include "components/component.h"
#include "renderer/spriteRenderer.h"
#include "world/world.h"
#include "renderer/debugRenderer.h"
#include "renderer/tileMapRenderer.h"
#include "renderer/fontRenderer.h"
#include "renderer/framebuffer.h"

struct LightingData{
    bool day;

    glm::vec3 dayColor;
    glm::vec3 nightColor;
    float interpolationThreshold;

    std::vector<glm::vec2> torches;
    Component::Transform* playerTransform;
    bool playerLight = false;

    float dayPeriod;
    float currentTime;

    float maxAmbientStrengthDay;
    float minAmbientStrengthDay;
    float maxAmbientStrengthNight;
    float minAmbientStrengthNight;

    void addTorch(const glm::vec2& f){
        torches.push_back(f);
    }

    void removeTorch(const glm::vec2& f){
        //dont remove the player torch
        for (int i = torches.size() - 1; i >= 0; i--){
            if (torches[i] == f){
                torches.erase(torches.begin() + i);
                return;
            }
        }
    }

    void addPlayerLight(){
        playerLight = true;
    }

    void removePlayerLight(){
        playerLight = false;
    }
};

class Renderer{
    public:
        Renderer();
        Renderer(Renderer& other) = delete;
        Renderer(const Renderer& other) = delete;
        Renderer(Renderer&& other) = delete;
        ~Renderer();

        void render();
        void init(World* world_);
        void remove(Terra::Entity& ent);
        void submit(Terra::Entity& ent);

        inline static LightingData& getLightingData(){ return instance->lightingData; }

    private:
        void renderScene();
        void renderWorld();
        void updateWorldData();
        void addTileData(int index, int x, int y, int texID);
        void addIndexData();
        void interpolateColorAndAmbience(glm::vec3& ambientColor, float& ambientStrength);

        std::vector<Scoped<SpriteRenderer>> spriteRenderers; 
        TileMapRenderer* tileMapRenderer;
        DebugRenderer* debugRenderer;
        FontRenderer* fontRenderer;

        FrameBuffer frameBuffer;
        Buffer frameBufferVBO;
        VertexArrayObject frameBufferVAO;
        Ref<Shader> frameBufferShader;

        LightingData lightingData;

        static Renderer* instance;

};