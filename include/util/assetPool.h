#pragma once

#include "renderer/texture.h"
#include "renderer/shader.h"
#include <unordered_map>

class AssetPool{
    public:
        AssetPool();
        AssetPool(const AssetPool& other) = delete;
        AssetPool(AssetPool& other) = delete;
        AssetPool(AssetPool&& other) = delete;
        ~AssetPool();

        static Texture& getTexture(const char* filePath, TexParams params = {GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR}, bool generateMipMaps = false);
        static Shader& getShader(const char* filePath);
    private:
        static AssetPool* instance;

        std::unordered_map<const char*, Texture> textures;
        std::unordered_map<const char*, Shader> shaders;
};