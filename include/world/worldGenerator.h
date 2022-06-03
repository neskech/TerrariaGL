#pragma once
#include "scene/Biome.h"
#include "scene/noise.h"
#include "constants.h"


struct Chunk{
    uint8_t tiles[CHUNK_WIDTH * CHUNK_HEIGHT];
};