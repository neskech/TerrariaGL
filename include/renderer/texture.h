#pragma once
#include "pch.h"

struct TexParams{
    int behaviorX, behaviorY;
    int minFilter, magFilter;
    int mipMapLevels;
};

class Texture{
    public:
        Texture(const char* assetPath, const TexParams& params);
        Texture(Texture& other) = delete;
        Texture(const Texture& other) = delete;
        Texture(Texture&& other) = delete;
        ~Texture();

        inline void bind() { glBindTexture(GL_TEXTURE_2D, ID); }
        inline void unBind() { glBindTexture(GL_TEXTURE_2D, 0); }
        inline unsigned int getID(){ return ID; }
        inline int getWidth() const{ return width; }
        inline int getHeight() const{ return height; }
        inline int getMipMapLevels() const{ return mipMapLevels; }

    private:
        bool generate(const char* assetPath, const TexParams& params);

        unsigned int ID;
        int width, height;
        int mipMapLevels;
};