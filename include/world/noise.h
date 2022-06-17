#pragma once
#include "pch.h"


//                                     SETTINGS FOR CREATING BIOMES USING NOISEMAPS                                                     //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct NoiseSettings{
    bool useDefault;

    uint16_t octaves;
    float frequency;
    float persistance;
    float lacrinarity;
    FastNoiseLite::NoiseType type;

    NoiseSettings(uint16_t octaves_, float persistance_, float lacrinarity_, float frequency_ = 1.0f, FastNoiseLite::NoiseType type_ = FastNoiseLite::NoiseType::NoiseType_OpenSimplex2S): 
        useDefault(false), octaves(octaves_), frequency(frequency_),
        persistance(persistance_), lacrinarity(lacrinarity_), type(type_)
    {}

    NoiseSettings(bool useDefault_): useDefault(useDefault_)
    {}

    NoiseSettings(){}        
};


void setNoiseSettings(NoiseSettings& settings);
void initNoise(int seed = -1);
float sampleNoise(float x, float y);