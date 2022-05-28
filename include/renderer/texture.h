#pragma once
#include "pch.h"

struct TexParams{
    int behaviorX, behaviorY;
    int minFilter, magFilter;
};

class Texture{
    public:
        Texture(const char* assetPath, TexParams params, bool generateMipMaps);
        Texture(Texture& other) = delete;
        Texture(const Texture& other) = delete;
        Texture(Texture&& other) = delete;

        inline void bind() { glBindTexture(GL_TEXTURE_2D, ID); }
        inline void unBind() { glBindTexture(GL_TEXTURE_2D, ID); }
        inline unsigned int getID(){ return ID; }

    private:
        bool generate(const char* assetPath, TexParams params, bool generateMipMaps);

        unsigned int ID;
        uint32_t width, height;
};