
#include "image.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Ref<Image> loadImage(const char* path, int reqComps){
    Ref<Image> img = std::make_shared<Image>();
    img->pixels = stbi_load(path, &img->width, &img->height, &img->channels, reqComps);
    return img;
}

Image* loadImageAsRaw(const char* path, int reqComps){
    Image* img = new Image{};
    img->pixels = stbi_load(path, &img->width, &img->height, &img->channels, reqComps);
    return img;
}