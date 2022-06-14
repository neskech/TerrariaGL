#pragma once
#include "components/component.h"
#include "renderer/spriteRenderer.h"
#include "world/world.h"
#include "renderer/debugRenderer.h"
#include "renderer/tileMapRenderer.h"

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

    private:
        void renderWorld();
        void updateWorldData();
        void addTileData(int index, int x, int y, int texID);
        void addIndexData();

        std::vector<Scoped<SpriteRenderer>> spriteRenderers; 
        TileMapRenderer* tileMapRenderer;
        DebugRenderer* debugRenderer;

        static Renderer* instance;

};