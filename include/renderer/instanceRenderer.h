#pragma once
#include "pch.h"
#include "components/component.h"
#include "renderer/shader.h"
#include "renderer/buffer.h"
#include "renderer/VAO.h"
#include <unordered_set>

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
        void addSpriteRenderer(Component::SpriteRenderer& spr);
        void removeSpriteRenderer(Component::SpriteRenderer& spr);
        void updateTransformData(const Component::SpriteRenderer& spr) const;
        void updateUVData(const Component::SpriteRenderer& spr) const;
        void updateTexIDData(const Component::SpriteRenderer& spr, int16_t texID) const;
        bool containsTexture(const Ref<Texture>& tex) const;

        inline int getCurrentSize() const { return numSprites; }
        inline int getnumTextures() const { return numTextures; }
    private:
        Component::SpriteRenderer* renderers[MAX_INSTANCES];
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
};