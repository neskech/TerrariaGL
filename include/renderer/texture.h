#pragma once
#include <stdlib.h>
#include <glad/glad.h>

struct TexParams{
    int behaviorX, behaviorY;
    int minFilter, magFilter;
};

class Texture{
    public:
        Texture(const char* assetPath);
        Texture(const Texture& other) = delete;
        Texture(Texture&& other) = delete;

        bool generate(TexParams params = {GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR}, bool generateMipMaps = false);
        inline void bind() { glBindTexture(GL_TEXTURE_2D, ID); }
        inline void unBind() { glBindTexture(GL_TEXTURE_2D, ID); }
        inline unsigned int getID(){ return ID; }

    private:
        unsigned int ID;
        uint32_t width, height;
        const char* path;
};