#pragma once
#include <stdlib.h>
#include "pch.h"


struct Image{
    int width, height, channels;
    uint8_t* pixels;

    ~Image(){
        if (pixels != nullptr)
            delete[] pixels;
    }
};

Ref<Image> loadImage(const char* path, int reqComps = 0);
Scoped<Image> loadImageAsRaw(const char* path, int reqComps = 0);