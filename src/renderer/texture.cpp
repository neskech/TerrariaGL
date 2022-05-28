#include "renderer/texture.h"
#include "util/image.h"

Texture::Texture(const char* assetPath, TexParams params, bool generateMipMaps){
    if (!generate(assetPath, params, generateMipMaps))
        std::cerr << "ERROR: In Texture constructor. Texture generation was unsuccessful!\n";
}

Texture::~Texture(){
    glDeleteTextures(1, &ID);
}

bool Texture::generate(const char* assetPath, TexParams params, bool generateMipmaps){
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, params.behaviorX);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, params.behaviorY);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, params.minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, params.magFilter);

    Scoped<Image> img = loadImageAsRaw(assetPath);
    if (img->pixels == nullptr){
        std::cerr << "ERROR: In Texture::generate. Failed to load image from path " << assetPath << '\n';
        return false;
    }

    width = img->width;
    height = img->height;

    if (img->channels == 3)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img->pixels);
    else if (img->channels == 4)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->pixels);
    
    if (generateMipmaps)
        glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
    return true;
}