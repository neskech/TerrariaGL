#pragma once
#include "pch.h"

struct NoiseSettings{
    bool useDefault;

    int seed;
    uint16_t octaves;
    float frequency;
    float persistance;
    float lacrinarity;
    FastNoiseLite::NoiseType type;

    NoiseSettings(int seed_, uint16_t octaves_, float persistance_, float lacrinarity_, float amplitude_ = 1.0f, float frequency_ = 1.0f, FastNoiseLite::NoiseType type_ = FastNoiseLite::NoiseType::NoiseType_OpenSimplex2S): 
        useDefault(false), seed(seed_), octaves(octaves_), frequency(frequency_),
        persitance(persistance_), lacrinarity(lacrinarity_), type(type_)
    {}

    NoiseSettings(bool useDefault_): useDefault(useDefault_)
    {}
        
        
};

const static NoiseSettings& lastAppliedSettings;

void setNoiseSettings(const NoiseSettings& settings);
float sampleNoise(float x, float y);