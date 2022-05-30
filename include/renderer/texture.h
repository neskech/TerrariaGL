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
        ~Texture();

        inline void bind() { glBindTexture(GL_TEXTURE_2D, ID); }
        inline void unBind() { glBindTexture(GL_TEXTURE_2D, 0); }
        inline unsigned int getID(){ return ID; }
        inline uint32_t getWidth(){ return width; }
        inline uint32_t getHeight(){ return height; }

    private:
        bool generate(const char* assetPath, TexParams params, bool generateMipMaps);

        unsigned int ID;
        uint32_t width, height;
};