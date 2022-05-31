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
        void updateUVData(Component::SpriteRenderer& spr, uint32_t index) const;
        void updateTexIDData(int16_t texID, uint32_t index) const;
        bool containsTexture(const Ref<Texture>& tex) const;
        void updateDirtyFlags();

        inline int getCurrentSize() const { return numSprites; }
        inline int getnumTextures() const { return numTextures; }
    private:
        Terra::Entity* entities[MAX_INSTANCES];
        std::unordered_map<Terra::Entity*, uint16_t> index_map;
        Texture* textures[MAX_TEXTURES];

        uint32_t numTextures;
        uint32_t numSprites;

        Buffer instance_transform_VBO;
        Buffer instance_uv_VBO;
        Buffer instance_texID_VBO;
        Buffer templateVBO;
        Buffer EBO;
        VertexArrayObject VAO;

        Ref<Shader> shader;

        friend class Renderer;
};