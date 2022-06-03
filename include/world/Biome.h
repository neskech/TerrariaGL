#pragma once
#include "pch.h"
#include "world/noise.h"
#include "constants.h"

#define _USE_MATH_DEFINES
#include <cmath>


//                                     SETTINGS FOR CREATING BIOMES USING NOISEMAPS                                                     //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define MAX(x, y) x > y ? x : y
#define MIN(x, y) x < y ? x : y
#define CLAMP(x, min, max) MIN(max, MAX(x, min))


enum Biome{
    forest = 0,
    snow = 1,
    desert = 2,
}

enum BlockType{
    air,
    dirt,
    stone,
    gravel,

    sand,
    sandstone,


    snow,
    packedSnow,
    ice,

    iron,
    silver,
    lead,
    diamond,
}

struct HeightModifier{
    bool constant;
    float maxMultiplier;
    int heightRange;
    float modifier; //Affects how quickly the function reaches the maxMultiplier
    float minHeight;

    HeightModifier(bool constant_): 
        constant(constant_)
    {}

    HeightModifier(float maxMultiplier_, int heightRange_, float modifier_, float minHeight_): 
        constant(false), maxMultiplier(maxMultiplier_), heightRange(heightRange_), modifier(modifier_), minHeight(minHeight_)
    {}

    float sample(float x){
       if (constant || x < minHeight)
            return 0.0f;
       maxMultiplier * ( M_E / (M_E - 1.0f) ) * ( 1.0f - exp( -x / (heightRange * modifier) ) );
    }
};

struct TileData{
    BlockType type;
    float proportion;
    HeightModifier modifier;
}

class BiomRule{
    public:
        virtual void init() = 0;
        virtual BlockType sampleBlock(float x, float y) = 0;
        float* getHeightMap(float startX);

        inline void setCaveMapSettings(const NoiseSettings& settings){ caveMap = settings; }
        inline void setTileMapSettings(const NoiseSettings& settings){ tileMap = settings; }
        inline void setHeightMapSettings(const NoiseSettings& settings){ heightMap = settings; }
        inline void setOreMapSettings(const NoiseSettings& settings){ oreMap = settings; }

    protected:
        NoiseSettings caveMap; //NoiseMap used to sample caves from
        NoiseSettings tileMap; //NoiseMap used to sample tiles from
        NoiseSettings heightMap; //NoiseMap (1D) used to determine surface terrain
        NoiseSettings oreMap; //NoiseMap for configuring where ores spawn

        HeightModifier caveModifier;
        const float oreCutoffProportion = 0.9f; //The value of the oreMap must be this to consider placing an ore
        const float caveCutoffProportion = 0.5f; //The value at which a cave will be activated
};

class ForestBiome : public BiomRule{
    public:
        void init();
        BlockType sampleBlock(float x, float y) override;
    private:        
        TileData baseTiles[2];
        TileData ores[4];
};

class SandBiome : public BiomRule{
    public:
        void init();
        BlockType sampleBlock(float x, float y) override;
    private:
        TileData baseTiles[2];

};

class SnowBiome : public BiomRule{
    public:
        void init();
        BlockType sampleBlock(float x, float y) override;
    private:
        TileData baseTiles[3];
        TileData ores[4];

};

