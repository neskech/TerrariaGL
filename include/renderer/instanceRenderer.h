#pragma once
#include "pch.h"
#include "components/component.h"
#include "renderer/shader.h"
#include "renderer/buffer.h"
#include "renderer/VAO.h"
#include <unordered_map>

#include "constants.h"

class InstanceRenderer{
    public:
        InstanceRenderer();
        InstanceRenderer(InstanceRenderer& other) = delete;
        InstanceRenderer(const InstanceRenderer& other) = delete;
        InstanceRenderer(InstanceRenderer&& other) = delete;
        ~InstanceRenderer();

        void init();
        void render();
        void addEntity(Terra::Entity& spr);
        void removeEntity(Terra::Entity& spr);
        void updateTransformData(Component::Transform& trans, uint32_t index) const;
        void updateTexCordData(Component::SpriteRenderer& spr, uint32_t index) const;
        void updateTexIDData(int16_t texID, uint32_t index) const;
        bool containsTexture(const Ref<Texture>& tex) const;
        void updateDirtyFlags();

        inline int getCurrentSize() const { return numSprites; }
        inline int getnumSpriteSheets() const { return numSpriteSheets; }
    private:
        Terra::Entity* entities[MAX_INSTANCES];
        std::unordered_map<Terra::Entity*, uint16_t> index_map;
        SpriteSheet* spriteSheets[MAX_TEXTURES]; //Spritesheets can be 1 sprite or multiple

        uint32_t numSpriteSheets;
        uint32_t numSprites;

        Buffer instance_transform_VBO;
        Buffer instance_texCord_VBO;
        Buffer instance_texID_VBO;
        Buffer templateVBO;
        Buffer EBO;
        VertexArrayObject VAO;

        Ref<Shader> shader;

        friend class Renderer;
};