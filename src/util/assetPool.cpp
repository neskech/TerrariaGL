#include "util/assetPool.h"

AssetPool* AssetPool::instance = nullptr;

AssetPool::AssetPool(){
    if (instance != nullptr)
        assert(false);

    instance = this;
}

Shader& AssetPool::getShader(const char* filePath){
    if (instance->shaders.find(filePath) == instance->shaders.end())
        instance->shaders.emplace(std::make_pair(filePath, Shader(filePath)));
    return instance->shaders[filePath];
}

Texture& AssetPool::getTexture(const char* filePath, TexParams params, bool generateMipMaps){
    if (instance->textures.find(filePath) == instance->textures.end())
        instance->textures.emplace(std::make_pair(filePath, Texture(filePath, params, generateMipMaps)));
    return instance->textures[filePath];
}