#include "world/Biome.h"



static BlockType getType(TileData* baseRanges, int length, float noiseValue, float heightValue){
    float augRanges[length];

    float range, extents;
    for (int i = 0; i < length; i++){
        range = baseRanges[i].proportion;  
        //Extend the range by a scalar value equally on both sides 
        extents = range * baseRanges[i].modifier.sample(heightValue) / 2.0f;
        augRanges[i] = range + extents;
        
        //Push the range before us backwards to make room for this current range after its expanded
        int a = i - 1;
        while (a >= 0 && extents > 0){
            float temp = augRanges[a];
            augRanges[a] = MAX(0.0f, augRanges[a] - extents);
            extents -= temp;
            a--;
        }

    }

    //normalize values between 0 and 1
    float sum;
    for (int i = 0; i < length; i++)
        sum += augRanges[i];
    for (int i = 0; i < length; i++)
        augRanges[i] /= sum;


    noiseValue = (noiseValue + 1.0f) / 2.0f;
    float runningSum = 0.0f;
    for (int a = 0; a < length; a++){
        if (noiseValue >= runningSum && noiseValue <= augRanges[a] + runningSum)
            return baseRanges[a].type;
        runningSum += augRanges[a];
    }

    return BlockType::air;
}

BiomeRule::BiomeRule(): 
    caveMap(true), tileMap(true), heightMap(true), oreMap(true), caveModifier(true)
{} 

Scoped<int[]> BiomeRule::getHeightMap(int startX){
    Scoped<int[]> heightM = std::make_unique<int[]>(CHUNK_WIDTH);
    setNoiseSettings(heightMap);

    for (int i = 0; i < CHUNK_WIDTH; i++){
        heightM[i] = sampleNoise(i + startX, 0.0f);
        heightM[i] = (int) ( surfaceAmplitude * ( (heightM[i] + 1.0f) / 2.0f ) );
    }
    return heightM;
}



ForestBiome::ForestBiome(): BiomeRule()
{
    baseTiles = new TileData[2];
    ores = new TileData[4];
}

ForestBiome::~ForestBiome(){
    delete[] baseTiles;
    delete[] ores;
}

void ForestBiome::init(){

    baseTiles[0] = {BlockType::dirt, .50f, HeightModifier(true)};
    baseTiles[1] = {BlockType::stone, .50f, HeightModifier(.90f, CHUNK_HEIGHT / 2, 1.0f, 0.0f)};

    ores[0] = {BlockType::iron, .40f, HeightModifier(.20f, CHUNK_HEIGHT / 2, 1.2f, 0 + CHUNK_HEIGHT / 10)};
    ores[1] = {BlockType::lead, .30f, HeightModifier(.30f, CHUNK_HEIGHT / 2, 1.2f, 0 + CHUNK_HEIGHT / 8)};
    ores[2] = {BlockType::silver, .20f, HeightModifier(.40f, CHUNK_HEIGHT / 2, 1.3f, 0 + CHUNK_HEIGHT / 6)};
    ores[3] = {BlockType::gold, .10f, HeightModifier(.40f, CHUNK_HEIGHT / 2, 1.4f, 0 + CHUNK_HEIGHT / 4)};

    caveModifier = HeightModifier{.30f, CHUNK_HEIGHT / 2, 1.5f, CHUNK_HEIGHT / 10};
}

BlockType ForestBiome::sampleBlock(int x, int y){
    setNoiseSettings(caveMap);
    float caveValue = sampleNoise(x, y) * (1.0f + caveModifier.sample(y));

    if (caveValue >= caveCutoffProportion)
        return BlockType::air;

    setNoiseSettings(oreMap);
    float oreValue = sampleNoise(x, y);

    if (oreValue >= oreCutoffProportion){
        float oreValueNormalized = (oreValue - oreCutoffProportion) / (1.0f - oreCutoffProportion);
        return getType(ores, 4, oreValueNormalized, y);
    }

    setNoiseSettings(tileMap);
    float tileValue = sampleNoise(x, y);

    return getType(baseTiles, 2, tileValue, y);
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

SandBiome::SandBiome(): BiomeRule()
{
    baseTiles = new TileData[2];
}

SandBiome::~SandBiome(){
    delete[] baseTiles;
}

void SandBiome::init(){
    baseTiles[0] = {BlockType::sand, .50f, HeightModifier(true)};
    baseTiles[1] = {BlockType::sandStone, .50f, HeightModifier(.90f, CHUNK_HEIGHT / 2, 1.0f, 0.0f)};
        

    caveModifier = HeightModifier(.30f, CHUNK_HEIGHT / 2, 1.5f, CHUNK_HEIGHT / 10);
}

BlockType SandBiome::sampleBlock(int x, int y){
    setNoiseSettings(caveMap);
    float caveValue = sampleNoise(x, y) * (1.0f + caveModifier.sample(y));

    if (caveValue >= caveCutoffProportion)
        return BlockType::air;

    setNoiseSettings(tileMap);
    float tileValue = sampleNoise(x, y);

    return getType(baseTiles, 2, tileValue, y);
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////


SnowBiome::SnowBiome(): BiomeRule()
{
    baseTiles = new TileData[3];
    ores = new TileData[4];
}

SnowBiome::~SnowBiome(){
    delete[] baseTiles;
    delete[] ores;
}

void SnowBiome::init(){
    
     baseTiles[0] = {BlockType::snow, .50f, HeightModifier(true)};
     baseTiles[1] = {BlockType::packedSnow, .30f, HeightModifier(.90f, CHUNK_HEIGHT / 2, 1.0f, 0.0f)};
     baseTiles[2] = {BlockType::ice, .20f, HeightModifier(.20f, CHUNK_HEIGHT / 2, 1.0f, 0.0f)};
        
   

      ores[0] = {BlockType::iron, .40f, HeightModifier(.20f, CHUNK_HEIGHT / 2, 1.2f, 0 + CHUNK_HEIGHT / 10)};
      ores[1] = {BlockType::lead, .30f, HeightModifier(.30f, CHUNK_HEIGHT / 2, 1.2f, 0 + CHUNK_HEIGHT / 8)};
      ores[2] = {BlockType::silver, .20f, HeightModifier(.40f, CHUNK_HEIGHT / 2, 1.3f, 0 + CHUNK_HEIGHT / 6)};
      ores[3] = {BlockType::gold, .10f, HeightModifier(.40f, CHUNK_HEIGHT / 2, 1.4f, 0 + CHUNK_HEIGHT / 4)};

    caveModifier = HeightModifier{.30f, CHUNK_HEIGHT / 2, 1.5f, CHUNK_HEIGHT / 10};
}

BlockType SnowBiome::sampleBlock(int x, int y){
    setNoiseSettings(caveMap);
    float caveValue = sampleNoise(x, y) * (1.0f + caveModifier.sample(y));

    if (caveValue >= caveCutoffProportion)
        return BlockType::air;

    setNoiseSettings(oreMap);
    float oreValue = sampleNoise(x, y);

    if (oreValue >= oreCutoffProportion){
        float oreValueNormalized = (oreValue - oreCutoffProportion) / (1.0f - oreCutoffProportion);
        return getType(ores, 4, oreValueNormalized, y);
    }

    setNoiseSettings(tileMap);
    float tileValue = sampleNoise(x, y);

    return getType(baseTiles, 4, tileValue, y);
}