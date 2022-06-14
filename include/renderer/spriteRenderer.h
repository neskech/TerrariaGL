#pragma once
#include "pch.h"
#include "components/component.h"
#include "renderer/shader.h"
#include "renderer/buffer.h"
#include "renderer/VAO.h"

#include "constants.h"

class SpriteRenderer{
    public:
        SpriteRenderer(Renderer& renderer, int zIndex = 0);
        SpriteRenderer(SpriteRenderer& other) = delete;
        SpriteRenderer(const SpriteRenderer& other) = delete;
        SpriteRenderer(SpriteRenderer&& other) = delete;
        ~SpriteRenderer();

        void init();
        void render();
        void addEntity(Terra::Entity& spr);
        void removeEntity(Terra::Entity& spr);

        bool containsTexture(const Component::SpriteRenderer& spr);

        inline int getCurrentSize() const { return numSprites; }
        inline int getNumSpriteSheets() const { return numSpriteSheets; }
        inline int getNumSprites() const { return numSprites; }
        inline bool hasRoomSprites() const { return numSprites < MAX_INSTANCES / MAX_INSTANCE_RENDERERS; }
        inline bool hasRoomTextures() const { return numSpriteSheets < MAX_TEXTURES; }
        inline bool containsEntity(Terra::Entity& ent) const { return index_map.find(&ent) != index_map.end(); }
        inline int getZIndex() const { return zIndex; }
    private:
        void updateTransformData(Component::Transform& trans, uint32_t index);
        void updateColorData(Component::SpriteRenderer& spr, uint32_t index);
        void updateTexCordData(Component::SpriteRenderer& spr, uint32_t index);
        void updateTexIDData(int16_t texID, uint32_t index);
        void updateDirtyFlags();

        uint32_t numSprites;
        uint32_t numSpriteSheets;

        int zIndex;

        Buffer instanceVBO;
        Buffer templateVBO;
        Buffer templateEBO;
        VertexArrayObject VAO;

        Terra::Entity* entities[MAX_INSTANCES / MAX_INSTANCE_RENDERERS];
        SpriteSheet* spriteSheets[MAX_TEXTURES]; //Spritesheets can be 1 sprite or multiple
        std::unordered_map<Terra::Entity*, uint16_t> index_map;

        Ref<Shader> shader;

        Renderer& renderer;
        friend class Renderer;
};