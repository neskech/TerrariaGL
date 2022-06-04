#pragma once
#include "constants.h"
#include "world/noise.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>


//                                     SETTINGS FOR CREATING BIOMES USING NOISEMAPS                                                     //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




enum Biome{
    Forest = 0,
    Snow = 1,
    Desert = 2,
    NONE = 3,
};

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

    mud,
    lava,
    water,

    copper,
    iron,
    silver,
    lead,
    gold,
    diamond,
};

struct HeightModifier{
    bool constant;
    float maxMultiplier;
    int heightRange;
    float modifier; //Affects how quickly the function reaches the maxMultiplier
    float minHeight;
    float maxHeight;

    HeightModifier(){}
    
    HeightModifier(bool constant_): 
        constant(constant_)
    {}

    HeightModifier(float maxMultiplier_, int heightRange_, float modifier_, float minHeight_, float maxHeight_ = -1): 
        constant(false), maxMultiplier(maxMultiplier_), heightRange(heightRange_), modifier(modifier_), minHeight(minHeight_), maxHeight(maxHeight_)
    {}

    float sample(float x){
       if (constant || x < minHeight || (maxHeight != -1 && x > maxHeight))
            return 0.0f;

       return maxMultiplier * ( M_E / (M_E - 1.0f) ) * ( 1.0f - exp( -x / (heightRange * modifier) ) );
    }
};

struct TileData{
    BlockType type;
    float proportion;
    HeightModifier modifier;
    TileData(){}
    TileData(BlockType type_, float proportion_, const HeightModifier& modifier_): type(type_), proportion(proportion_), modifier(modifier_) {}
};

class BiomeRule{
    public:
        BiomeRule();
        virtual ~BiomeRule(){}
        virtual void init(){}
        virtual BlockType sampleBlock(int x, int y){return BlockType::air;};
        std::vector<float>* getHeightMap(int startX);

        inline void setCaveMapSettings(const NoiseSettings& settings){ caveMap = settings; }
        inline void setTileMapSettings(const NoiseSettings& settings){ tileMap = settings; }
        inline void setHeightMapSettings(const NoiseSettings& settings){ heightMap = settings; }
        inline void setOreMapSettings(const NoiseSettings& settings){ oreMap = settings; }
        inline void setSurfaceAmplitude(int amplitude){ surfaceAmplitude = amplitude; }

    protected:
        NoiseSettings caveMap; //NoiseMap used to sample caves from
        NoiseSettings tileMap; //NoiseMap used to sample tiles from
        NoiseSettings heightMap; //NoiseMap (1D) used to determine surface terrain
        NoiseSettings oreMap; //NoiseMap for configuring where ores spawn

        HeightModifier caveModifier;
        const float oreCutoffProportion = 0.9f; //The value of the oreMap must be this to consider placing an ore
        const float caveCutoffProportion = 0.8f; //The value at which a cave will be activated
        int surfaceAmplitude = 10.0f;
};

class ForestBiome : public BiomeRule{
    public:
        ForestBiome();
        ~ForestBiome(){}
        void init() override;
        BlockType sampleBlock(int x, int y) override;
    private:        
        TileData baseTiles[2]; //2
        TileData ores[4]; //4
};

class SandBiome : public BiomeRule{
    public:
        SandBiome();
        ~SandBiome(){}
        void init() override;
        BlockType sampleBlock(int x, int y) override;
    private:
        TileData baseTiles[2];

};

class SnowBiome : public BiomeRule{
    public:
        SnowBiome();
        ~SnowBiome(){}
        void init() override;
        BlockType sampleBlock(int x, int y) override;
    private:
        TileData baseTiles[3]; //3
        TileData ores[4]; //4

};

