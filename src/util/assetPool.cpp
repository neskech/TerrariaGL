#include "util/assetPool.h"

AssetPool* AssetPool::instance = nullptr;

AssetPool::AssetPool(){
    if (instance != nullptr)
        assert(false);

    instance = this;
}

AssetPool::~AssetPool(){
    
}

Ref<Shader>& AssetPool::getShader(const char* filePath){
    if (instance->shaders.find(filePath) == instance->shaders.end()){
        Ref<Shader> s = std::make_shared<Shader>(filePath);
        instance->shaders[filePath] = std::move(s);
    }
    return instance->shaders[filePath];
}

Ref<Texture>& AssetPool::getTexture(const char* filePath, TexParams params){
    if (instance->textures.find(filePath) == instance->textures.end()){
        Ref<Texture> t = std::make_shared<Texture>(filePath, params);
        instance->textures[filePath] = std::move(t);
    }
    return instance->textures[filePath];
}