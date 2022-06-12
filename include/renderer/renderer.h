#pragma once
#include "components/component.h"
#include "renderer/instanceRenderer.h"
#include "world/world.h"
#include "renderer/debugRenderer.h"

class Renderer{
    public:
        Renderer(World* world_);
        Renderer(Renderer& other) = delete;
        Renderer(const Renderer& other) = delete;
        Renderer(Renderer&& other) = delete;
        ~Renderer();

        static void render();
        static void init();
        static void remove(Terra::Entity& ent);
        static void submit(Terra::Entity& ent);

    private:
        static void renderWorld();
        static void updateWorldData();
        static void addTileData(int indexg, int x, int y, int texID);
        static void addIndexData();

        InstanceRenderer* instanceRenderer; //TODO rename to spriteRenderer
        DebugRenderer* debugRenderer;

        World* world;
        Buffer worldVBO;
        Buffer worldEBO;
        VertexArrayObject worldVAO;
        Ref<Shader> worldShader;
        Ref<Texture> tileAtlas;
        int numWorkingTiles;

        static Renderer* instance;

};