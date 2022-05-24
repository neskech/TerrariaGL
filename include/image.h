#pragma once
#include <stdlib.h>
#include "pch.h"


struct Image{
    int width, height, channels;
    uint8_t* pixels;
};

Ref<Image> LoadImage(const char* path, int reqComps = 0);