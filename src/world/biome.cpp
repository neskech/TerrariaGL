#include "world/Biome.h"
#include "pch.h"



static TileType getType(TileData* baseRanges, int length, float noiseValue, float heightValue){
    float augRanges[length];

    float range, extents;
    for (int i = 0; i < length; i++){
        range = baseRanges[i].proportion;  
        //Extend the range by a scalar value equally on both sides 
        extents =  baseRanges[i].modifier.sampleWithRange(heightValue, baseRanges[i].proportion) / 2.0f;
        augRanges[i] = range + extents;
        
        bool negative = false;
        if (augRanges[i] < 0){
            negative = true;
           // extents += augRanges[i];
            augRanges[i] = 0;
        }

        //Push the range before us backwards to make room for this current range after its expanded
        int a = i - 1;
        while (a >= 0 && ( (negative && extents < 0) || (!negative && extents > 0) ) ){
            float temp = augRanges[a];
            augRanges[a] = MAX(0.0f, augRanges[a] - extents);
            extents += extents < 0 ? temp : -temp;
            a--;
        }

    }
    //normalize values between 0 and 1
    float sum = 0.0f;
    for (int i = 0; i < length; i++)
        sum += augRanges[i];

    sum += sum == 0.0f ? 1.0f : 0.0f;
    for (int i = 0; i < length; i++)
        augRanges[i] /= sum;

    noiseValue = (noiseValue + 1.0f) / 2.0f;
    float runningSum = 0.0f;
    for (int a = 0; a < length; a++){
        if (noiseValue >= runningSum && noiseValue <= augRanges[a] + runningSum)
            return baseRanges[a].type;
        runningSum += augRanges[a];
    }

    for (int i = 0; i < length; i++)
        std::cout << baseRanges[i].modifier.modifier << " base " << baseRanges[i].proportion << " aug " << augRanges[i] <<  " height " << heightValue <<  " noise "  << noiseValue <<  " sample " << baseRanges[i].modifier.sampleWithRange(heightValue, baseRanges[i].proportion) <<  std::endl;

    assert(false);
    return TileType::air;
}

BiomeRule::BiomeRule(): 
    caveMap(true), tileMap(true), heightMap(true), oreMap(true), caveModifier(true)
{
    heightMap = NoiseSettings(5, 0.2f, 0.5f, 0.03f, FastNoiseLite::NoiseType::NoiseType_OpenSimplex2S);
} 

int* BiomeRule::getHeightMap(int startX){
    int* heightM = new int[CHUNK_WIDTH];
    setNoiseSettings(heightMap);

    for (int i = 0; i < CHUNK_WIDTH; i++){
        float val =  sampleNoise(i + startX + 0.1f, 0.0f);
        heightM[i] = (int) ( surfaceAmplitude * ( (val + 1.0f) / 2.0f ) );
    }
    return heightM;
}



ForestBiome::ForestBiome(): BiomeRule()
{}

void ForestBiome::init(){
    baseTiles[0] = TileData(TileType::dirt, .70f, HeightModifier(-.25f, CHUNK_HEIGHT / 3, 0.8f, -1.0f));
    baseTiles[1] = TileData(TileType::stone, .30f, HeightModifier(.70f, CHUNK_HEIGHT / 3, 1.2f, 20.0f));

    oreCutoffProportion = 0.85f;
    minOreHeight = 7;
    
    ores[0] = TileData(TileType::iron, .40f, HeightModifier(-.50f, CHUNK_HEIGHT / 2, 1.2f, 3));
    ores[1] = TileData(TileType::lead, .30f, HeightModifier(.30f, CHUNK_HEIGHT / 2, 1.2f, 0 + CHUNK_HEIGHT / 8));
    ores[2] = TileData(TileType::silver, .20f, HeightModifier(.40f, CHUNK_HEIGHT / 2, 1.3f, 0 + CHUNK_HEIGHT / 6));
    ores[3] = TileData(TileType::gold, .10f, HeightModifier(.40f, CHUNK_HEIGHT / 2, 1.4f, 0 + CHUNK_HEIGHT / 4));

    caveModifier = HeightModifier{.20f, CHUNK_HEIGHT / 2, 0.9f, 20.0f};
    
    caveMap = NoiseSettings(5, 0.8f, 0.3f, 0.03f);

    oreMap = NoiseSettings(5, 0.8f, 0.03f, 0.05f);

}

TileType ForestBiome::sampleBlock(int x, int y){
    setNoiseSettings(caveMap);
    float caveValue = sampleNoise(x + 0.1f, y + 0.1f);
    caveValue =  (1.0f + caveModifier.sample(y)) * ((caveValue + 1.0f) / 2.0f);

    if (caveValue >= caveCutoffProportion)
        return TileType::air;

    if (y >= minOreHeight){
        setNoiseSettings(oreMap);
        float oreValue = sampleNoise(x + 0.1f, y + 0.1f);
        oreValue = (oreValue + 1.0f) / 2.0f;

        if (oreValue >= oreCutoffProportion){
            float oreValueNormalized = (oreValue - oreCutoffProportion) / (1.0f - oreCutoffProportion);
            return getType(ores, 4, oreValueNormalized, y);
        }
    }

    setNoiseSettings(tileMap);
    float tileValue = sampleNoise(x + 0.1f, y + 0.1f);
    return getType(baseTiles, 2, tileValue, y);
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

SandBiome::SandBiome(): BiomeRule()
{}

void SandBiome::init(){
    baseTiles[0] = TileData(TileType::sand, .80f, HeightModifier(-.30f, CHUNK_HEIGHT / 3, 0.8f, -1.0f));
    baseTiles[1] = TileData(TileType::sandstone, .20f, HeightModifier(.90f, CHUNK_HEIGHT / 2, 1.0f, 0.0f));
        
    caveCutoffProportion = 0.8f;
    caveModifier = HeightModifier(.10f, CHUNK_HEIGHT / 2, 1.5f, CHUNK_HEIGHT / 10);
    heightMap.frequency = 0.03f;
    heightMap.persistance = 1.5f;
    heightMap.lacrinarity = 3.8f;

    //caveMap = NoiseSettings(5, 0.8f, 0.3f, 0.03f);
}

TileType SandBiome::sampleBlock(int x, int y){
    setNoiseSettings(caveMap);
    float caveValue = sampleNoise(x, y) * (1.0f + caveModifier.sample(y));

    if (caveValue >= caveCutoffProportion)
        return TileType::air;

    setNoiseSettings(tileMap);
    float tileValue = sampleNoise(x, y);

    return getType(baseTiles, 2, tileValue, y);
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////


SnowBiome::SnowBiome(): BiomeRule()
{}

void SnowBiome::init(){
     surfaceAmplitude = 20;

     baseTiles[0] = TileData(TileType::snow, .15f, HeightModifier(-.30f, CHUNK_HEIGHT / 3, 0.8f, 0.0f));
     baseTiles[1] = TileData(TileType::packedSnow, .05f, HeightModifier(.20f, CHUNK_HEIGHT / 2, 1.0f, 10.0f));
     baseTiles[2] = TileData(TileType::ice, .80f, HeightModifier(.30f, CHUNK_HEIGHT / 2, 1.0f, 0.0f));
    
     minOreHeight = 15;
     ores[0] =  TileData(TileType::lead, .40f, HeightModifier(-.30f, CHUNK_HEIGHT / 2, 1.2f, minOreHeight + 0));
     ores[1] =  TileData(TileType::silver, .20f, HeightModifier(-.40f, CHUNK_HEIGHT / 2, 1.3f, minOreHeight + 2));
     ores[2] =  TileData(TileType::gold, .30f, HeightModifier(.40f, CHUNK_HEIGHT / 2, 1.4f, minOreHeight + 6));
     ores[3] =  TileData(TileType::diamond, .10f, HeightModifier(.60f, CHUNK_HEIGHT / 2, 1.4f, minOreHeight + 10));

     caveModifier = HeightModifier{.50f, CHUNK_HEIGHT / 2, 1.5f, 0};
}

TileType SnowBiome::sampleBlock(int x, int y){
    setNoiseSettings(caveMap);
    float caveValue = sampleNoise(x, y) * (1.0f + caveModifier.sample(y));

    if (caveValue >= caveCutoffProportion)
        return TileType::air;

    if (y >= minOreHeight){
        setNoiseSettings(oreMap);
        float oreValue = sampleNoise(x, y);

        if (oreValue >= oreCutoffProportion){
            float oreValueNormalized = (oreValue - oreCutoffProportion) / (1.0f - oreCutoffProportion);
            return getType(ores, 4, oreValueNormalized, y);
        }
    }

    setNoiseSettings(tileMap);
    float tileValue = sampleNoise(x, y);

    return getType(baseTiles, 3, tileValue, y);
}