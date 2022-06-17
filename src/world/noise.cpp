#include "world/noise.h"
#include "pch.h"

static FastNoiseLite noise;
static NoiseSettings* lastAppliedSettings = nullptr;

float sampleNoise(float x, float y){
    return noise.GetNoise<float>(x, y);
}

void initNoise(int seed){
    srand(time(NULL));
    if (seed == -1)
        seed = rand();
    noise.SetSeed(seed);
}

void setNoiseSettings(NoiseSettings& settings){
    if (lastAppliedSettings == &settings)
        return;

    else if (settings.useDefault){
        noise.SetNoiseType(FastNoiseLite::NoiseType::NoiseType_OpenSimplex2);
        noise.SetFrequency(0.01f);
        noise.SetFractalOctaves(3);
        noise.SetFractalGain(0.5f);
        noise.SetFractalLacunarity(2.0f);
        lastAppliedSettings = &settings;
    
    }

    else{
        noise.SetNoiseType(settings.type);
        noise.SetFrequency(settings.frequency);
        noise.SetFractalOctaves(settings.octaves);
        noise.SetFractalGain(settings.persistance);
        noise.SetFractalLacunarity(settings.lacrinarity);
        lastAppliedSettings = &settings;
    }
}
