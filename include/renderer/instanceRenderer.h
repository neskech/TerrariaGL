#pragma once
#include "pch.h"
#include "components/component.h"
#include "renderer/shader.h"
#include "renderer/buffer.h"
#include "renderer/VAO.h"
#include <unordered_map>

#include "constants.h"
static constexpr int VERTEX_SIZE = 19;

class InstanceRenderer{
    public:
        InstanceRenderer();
        ~InstanceRenderer();

        void init();
        void render();
        void addSpriteRenderer(Component::SpriteRenderer* spr);
        void removeSpriteRenderer(Component::SpriteRenderer* spr);
        void updateDirtyFlags();
        void configureSpriteVertexData(Component::SpriteRenderer* spr, int16_t texID);
        bool containsTexture(const Ref<Texture>& tex);

        inline int getCurrentSize(){ return numSprites; }
        inline int getnumTextures(){ return numTextures; }
        inline std::unordered_map<Component::SpriteRenderer*, uint16_t>& getRenderers(){ return spriteRends; }
    private:
        std::unordered_map<Component::SpriteRenderer*, uint16_t> spriteRends;
        Texture* textures[MAX_TEXTURES];

        int numTextures;
        std::size_t numSprites;

        Buffer* instance_transform_VBO;
        Buffer* instance_uv_VBO;
        Buffer* instance_texID_VBO;
        Buffer* templateVBO;
        VertexArrayObject* VAO;
        Ref<Shader> shader;
};