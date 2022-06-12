#pragma once

#include "renderer/texture.h"
#include "renderer/shader.h"
#include "pch.h"

class AssetPool{
    public:
        AssetPool();
        AssetPool(const AssetPool& other) = delete;
        AssetPool(AssetPool& other) = delete;
        AssetPool(AssetPool&& other) = delete;
        ~AssetPool();

        static Ref<Texture>& getTexture(const char* filePath, TexParams params = {GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, 1});
        static Ref<Shader>& getShader(const char* filePath);
    private:
        static AssetPool* instance;

        std::unordered_map<std::string, Ref<Texture>> textures;
        std::unordered_map<std::string, Ref<Shader>> shaders;
};