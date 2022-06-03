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

    

Scoped<float> BiomRule::getHeightMap(float startX){
    Scoped<float> heightMap = std::make_unique<float[]>(CHUNK_WIDTH);
    setNoiseSettings(heightMap);
    for (int i = 0; i < CHUNK_WIDTH; i++){
        heightMap[i] = sampleNoise(i + startX, 0.0f);
    }
    return heightMap;
}



ForestBiome::ForestBiome(): 
    caveMap(true), tileMap(true), oreMap(true), heightMap(true)
{}

void ForestBiome::init()
    baseTiles = {
        {BlockType::dirt, .35f, HeightModifier(true)},
        {BlockType::stone}, .35f, HeightModifier(.90f, CHUNK_HEIGHT / 2, 1.0f, 0.0f) },
    }

    ores = {
        {BlockType::iron, .40f, HeightModifier(.20f, CHUNK_HEIGHT / 2, 1.2f, 0 + CHUNK_HEIGHT / 10)},
        {BlockType::lead, .30f, HeightModifier(.30f, CHUNK_HEIGHT / 2, 1.2f, 0 + CHUNK_HEIGHT / 8)},
        {BlockType::silver, .20f, HeightModifier(.40f, CHUNK_HEIGHT / 2, 1.3f, 0 + CHUNK_HEIGHT / 6)},
        {BlockType::gold, .10f, HeightModifier(.40f, CHUNK_HEIGHT / 2, 1.4f, 0 + CHUNK_HEIGHT / 4)},
    }

    caveModifier = HeightModifier{.30f, CHUNK_HEIGHT / 2, 1.5f, CHUNK_HEIGHT / 10};
}

BlockType ForestBiome::sampleBlock(float x, float y){
    setNoiseSettings(caveMap);
    float caveValue = sampleNoise(x, y) * caveModifier.sample(y);

    if (caveValue >= caveCutoffProp)
        return BlockType::air;

    setNoiseSettings(oreMap);
    float oreValue = sampleNoise(x, y);

    if (oreValue >= oreCutoffProportion){
        float oreValueNormalized = (oreValue - oreCutoffProportion) / (1.0f - oreCutoffProportion);
        return getType(ores, oreValueNormalized), y);
    }

    setNoiseSettings(tileMap);
    float tileValue = sampleNoise(x, y);

    return getType(baseTiles, tileValue, y);
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

SandBiome::SandBiome(): 
    caveMap(true), tileMap(true), oreMap(true), heightMap(true)
{}

void SandBiome::init()
    baseTiles = {
        {BlockType::sand, .50f, HeightModifier(true)},
        {BlockType::sandStone}, .50f, HeightModifier(.90f, CHUNK_HEIGHT / 2, 1.0f, 0.0f) },
        
    }

    caveModifier = HeightModifier{.30f, CHUNK_HEIGHT / 2, 1.5f, CHUNK_HEIGHT / 10};
}

BlockType SandBiome::sampleBlock(float x, float y){
    setNoiseSettings(caveMap);
    float caveValue = sampleNoise(x, y) * caveModifier.sample(y);

    if (caveValue >= caveCutoffProp)
        return BlockType::air;

    setNoiseSettings(tileMap);
    float tileValue = sampleNoise(x, y);

    return getType(baseTiles, tileValue, y);
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////


SnowBiome::SnowBiome(): 
    caveMap(true), tileMap(true), oreMap(true), heightMap(true)
{}

void SnowBiome::init()
    baseTiles = {
        {BlockType::snow, .50f, HeightModifier(true)},
        {BlockType::packed, .50f, HeightModifier(.90f, CHUNK_HEIGHT / 2, 1.0f, 0.0f)},
        
    }

    ores = {
        {BlockType::iron, .40f, HeightModifier(.20f, CHUNK_HEIGHT / 2, 1.2f, 0 + CHUNK_HEIGHT / 10)},
        {BlockType::lead, .30f, HeightModifier(.30f, CHUNK_HEIGHT / 2, 1.2f, 0 + CHUNK_HEIGHT / 8)},
        {BlockType::silver, .20f, HeightModifier(.40f, CHUNK_HEIGHT / 2, 1.3f, 0 + CHUNK_HEIGHT / 6)},
        {BlockType::gold, .10f, HeightModifier(.40f, CHUNK_HEIGHT / 2, 1.4f, 0 + CHUNK_HEIGHT / 4)},
    }

    caveModifier = HeightModifier{.30f, CHUNK_HEIGHT / 2, 1.5f, CHUNK_HEIGHT / 10};
}

BlockType SnowBiome::sampleBlock(float x, float y){
    setNoiseSettings(caveMap);
    float caveValue = sampleNoise(x, y) * caveModifier.sample(y);

    if (caveValue >= caveCutoffProp)
        return BlockType::air;

    setNoiseSettings(oreMap);
    float oreValue = sampleNoise(x, y);

    if (oreValue >= oreCutoffProportion){
        float oreValueNormalized = (oreValue - oreCutoffProportion) / (1.0f - oreCutoffProportion);
        return getType(ores, oreValueNormalized), y);
    }

    setNoiseSettings(tileMap);
    float tileValue = sampleNoise(x, y);

    return getType(baseTiles, tileValue, y);
}