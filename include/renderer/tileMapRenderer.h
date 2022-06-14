#pragma once
#include "world/world.h"
#include "renderer/shader.h"
#include "pch.h"
#include "renderer/texture.h"
#include "renderer/buffer.h"
#include "renderer/VAO.h"

class TileMapRenderer{
    public:
        TileMapRenderer(World* world_);
        TileMapRenderer(TileMapRenderer& other) = delete;
        TileMapRenderer(const TileMapRenderer& other) = delete;
        TileMapRenderer(TileMapRenderer&& other) = delete;
        ~TileMapRenderer();

        void render();
        void init();

    private:
        void renderWorld();
        void updateWorldData();
        void addTileData(int index, int x, int y, int texID);
        void addIndexData();

        World* world;
        Buffer VBO;
        Buffer EBO;
        VertexArrayObject VAO;
        Ref<Shader> shader;
        Ref<Texture> tileAtlas;
        int numWorkingTiles;

        static TileMapRenderer* instance;

        friend class World;
};