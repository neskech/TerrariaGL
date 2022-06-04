#include "world/world.h"
#include <time.h>  

static constexpr int MAX_CHUNKS = 11; //5 on each side, 1 in the middle
static constexpr int CHUNK_POP_SIZE = 3; //Amount of chunks to pop off one side and add to the other
static constexpr int POP_DISTANCE = (MAX_CHUNKS - 1) / 2; //Distance from the center at which to start popping chunks
static constexpr int MIN_BIOME_LENGTH = 6;

World* World::instance = nullptr;

World::World(): biomeBlending(true), currentChunkIndex(0), runningIndex(0){
    if (instance != nullptr)
        assert(false);
    instance = this;

    biomeGenerators = {
        new ForestBiome(),
        new SandBiome(),
        new SnowBiome(),
    };

    for (int i = 0; i < 3; i++)
        biomeGenerators[i]->init();
}

World::~World(){
    for (int i = 0; < 3; i++)
        delete biomeGenerators[i];
}

void World::init(){
   Chunk* base = generateChunk(Biome::Forest, Biome::NONE, Direction::NONE);
   chunks.addFront(base);
   currentChunkNode = chunks.getNode(0);

   rightGeneration{Biome::Forest, 5, 0};
   leftGeneration{Biome::Forest, 5, 0};

   Biome prevBiomeLeft = Biome::NONE;
   Biome prevBiomeRight = Biome::NONE;
   for (int i = 1; i <= (MAX_CHUNKS - 1) / 2; i++){
        if (rightGeneration.numsChunksGenerated == rightGeneration.biomeLengthChunks){
            prevBiomeRight = rightGeneration.type;
            generateNewBiomeData(rightGeneration);
        }

        if (leftGeneration.numsChunksGenerated == leftGeneration.biomeLengthChunks){
            prevBiomeLeft = leftGeneration.type;
            generateNewBiomeData(leftGeneration);
        }

        Chunk* right = generateChunk(rightGeneration.type, prevBiomeLeft, i, Direction::RIGHT);
        Chunk* left = generateChunk(leftGeneration.type, prevBiomeRight, -i, Direction::LEFT);

        chunks.addFront(left);
        chunks.addEnd(right);

        prevBiomeLeft = Biome::NONE;
        prevBiomeRight = Biome::NONE;
        rightGeneration.numsChunksGenerated++;
        leftGeneration.numsChunksGenerated++;
   }
}

void World::update(float playerPosX){
    //Update the current chunk Node and running index based on the player's position
    //If the player is X distance from the center, call the pop and append functions
    int chunkCord = (int) (playerPosX - playerPosX < 0 ? CHUNK_WIDTH : 0) / CHUNK_WIDTH; // offset such that left chunk starts at index -1
    if (chunkCord != currentChunkIndex){
        currentChunkNode = chunks.getNodeStartingFrom(currentChunkNode, getChunkIndexInArray(currentChunkIndex), getChunkIndexInArray(chunkCord));
        currentChunkIndex = chunkCord;
    }

    // if (abs(currentChunkIndex - runningIndex) >= POP_DISTANCE){
    //     Direction popDir = currentChunkIndex - runningIndex < 0 ? Direction::LEFT : Direction::RIGHT;
    //     Direction appendDir = popDir == Direction::LEFT ? Direction::RIGHT : Direction::LEFT;
    //     popChunks(popDir);
    //     appendChunks(appendDir);
    //     runningIndex += popDir == Direction::LEFT ? CHUNK_POP_SIZE : -CHUNK_POP_SIZE;
    // }
}

Chunk* World::generateChunk(Biome biome, Biome previousBiome, int index, Direction direction){
     Scoped<int[]> heightMap = biomeGenerators[biome]->generateHeightMap(index * CHUNK_WIDTH);
     int offset = index * CHUNK_WIDTH;

     Chunk* chunk = new Chunk(biome);
     for (int i = 0; i < CHUNK_WIDTH; i++){
         for (int j = heightMap[i]; j > -CHUNK_HEIGHT / 2; j--){
            
             BlockType block;

             bool failed = true;
             if (previousBiome != Biome::NONE){
                 
                 //Check if we're within the blending distance
                 if ( (direction == Direction::RIGHT && i <= blendDistance) || (direction == Direction::LEFT && CHUNK_WIDTH - 1 - i <= blendDistance) ){

                    setNoiseSettings(biomeBlending);
                    float blendValue = sampleNoise(offset + i);
                    if (blendValue >= blendCutoffValue){
                        block = biomeGenerators[previousBiome]->sampleBlock(offset + i, j);
                        failed = false;
                    }

                 }
             }

             if (failed)
                 block = biomeGenerators[biome]->sampleBlock(offset + i, j);

            chunk.tiles[ abs( j - CHUNK_HEIGHT / 2 ) * CHUNK_WIDTH + i] = block;
         }
     }

     return chunk;
}

void World::popChunks(Direction direction){

}

void World::appendChunks(Direction direction){

}

void World::generateNewBiomeData(const BiomeGenData& data){
     srand(time(NULL));
     int length = rand() % 10;
     data.biomeLengthChunks = length + MIN_BIOME_LENGTH;
     data.numsChunksGenerated = 0;
     data.type = rand() % 3;
}

int World::getChunkIndexInArray(int worldChunkIndex){
    //Running index = world chunk index of the center of the list
    //Distance from the center + the center
    return (worldChunkIndex - runningIndex) + (MAX_CHUNKS - 1) / 2;
}