#pragma once
#include "components/component.h"
#include "renderer/renderBatch.h"
#include <vector>

constexpr std::size_t MAX_BATCH_SIZE = 1000;
constexpr int MAX_TEXTURES =  8;

class Renderer{
    public:
        Renderer();
        Renderer(Renderer& other) = delete;
        Renderer(const Renderer& other) = delete;
        Renderer(Renderer&& other) = delete;
        ~Renderer();

        static void render();
        static void deleteFromBatch(Component::SpriteRenderer& spr);
        static void addToBatch(Component::SpriteRenderer& spr);

    private:
        std::vector<RenderBatch*> batches;
        static Renderer* instance;
        friend class RenderBatch;
        
};
