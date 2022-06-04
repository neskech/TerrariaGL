#include "world/noise.h"

void setNoiseSettings(const NoiseSettings& settings){
    if (lastAppliedSettings == &settings || settings.useDefault)
        return;

    FastNoiseLite::SetNoiseType(settings.type);
    FastNoiseLite::SetFrequency(settings.frequency);
    FastNoiseLite::SetFractalOctaves(settings.octaves);
    FastNoiseLite::SetFractalGain(settings.persistance);
    FastNoiseLite::SetFractalLacunarity(settings.lacrinarity);
    FastNoiseLite::SetSeed(settings.seed);
    lastAppliedSettings = settings;
}

float sampleNosie(float x, float y){
    return FastNoiseLite::GetNoise<float>(x, y);
}