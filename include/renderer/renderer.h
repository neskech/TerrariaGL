#pragma once
#include <vector>
#include "renderer/renderBatch.h"

constexpr int MAX_BATCH_SIZE = 10_000;

class Renderer{
    public:
        Renderer();
        Renderer(Renderer& other) = delete;
        Renderer(const Renderer& other) = delete;
        Renderer(Renderer&& other) = delete;
        ~Renderer();

        static void render();
        static void addToBatch();

    private:
        std::vector<RenderBatch> batches;
        static Renderer* instance;
};

Renderer* Renderer::instance = nullptr;