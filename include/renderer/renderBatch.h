#pragma once
#include "pch.h"
#include "components/component.h"
#include "renderer/shader.h"
#include "renderer/buffer.h"
#include "renderer/VAO.h"
#include <unordered_map>

static constexpr int VERTEX_SIZE = 5;

class RenderBatch{
    public:
        RenderBatch();
        ~RenderBatch();

        void init();
        void render();
        void addSpriteRenderer(Component::SpriteRenderer* spr);
        void removeSpriteRenderer(Component::SpriteRenderer* spr);
        void generateIndices();
        void updateDirtyFlags();
        void configureSpriteVertexData(Component::SpriteRenderer* spr, int16_t texID);
        bool containsTexture(const Ref<Texture>& tex);

        inline int getCurrentSize(){ return numSprites; }
        inline int getnumTextures(){ return numTextures; }
        inline std::unordered_map<Component::SpriteRenderer*, uint16_t>& getRenderers(){ return spriteRends; }
    private:
        std::unordered_map<Component::SpriteRenderer*, uint16_t> spriteRends;
        Ref<Texture> textures[MAX_TEXTURES];

        int numTextures;
        std::size_t numSprites;

        Buffer VBO{BufferType::VBO};
        Buffer EBO{BufferType::EBO};
        VertexArrayObject VAO{sizeof(float) * 4};
        Ref<Shader> shader;
};