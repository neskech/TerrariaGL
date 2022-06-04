#include "world/noise.h"

static FastNoiseLite noise;
static NoiseSettings* lastAppliedSettings = nullptr;

float sampleNoise(float x, float y){
    return noise.GetNoise<float>(x, y);
}

void setNoiseSettings(NoiseSettings& settings){
    if (lastAppliedSettings == &settings || settings.useDefault)
        return;

    noise.SetNoiseType(settings.type);
    noise.SetFrequency(settings.frequency);
    noise.SetFractalOctaves(settings.octaves);
    noise.SetFractalGain(settings.persistance);
    noise.SetFractalLacunarity(settings.lacrinarity);
    noise.SetSeed(settings.seed);
    lastAppliedSettings = &settings;
}
