#include "world/world.h"
#include "scene/camera.h"
#include "constants.h"

#define CHUNK_BLEND_DISTANCE 10
#define CHUNK_BLEND_CUTOFF 0.5f

#define MAX_CHUNKS 11
#define CHUNK_POP_SIZE 3
#define CHUNK_POP_DISTANCE 3
#define MIN_BIOME_LENGTH 1

#define FLAURA_CUTOFF 0.70f
#define MIN_TREE_HEIGHT 4
#define MAX_TREE_HEIGHT 10


World* World::instance = nullptr;

World::World(): biomeBlending(true), currentChunkIndex(0), runningIndex(0){
    if (instance != nullptr)
        assert(false);

    instance = this;

    //MAX_CHUNKS must be an odd number > 3
    assert(MAX_CHUNKS > 3 && MAX_CHUNKS % 2 == 1);
    //CAM_WIDTH and CAM_HEIGHT must be < CHUNK_WIDTH, CHUNK_HEIGHT
    assert(CAM_WIDTH < CHUNK_WIDTH && CAM_HEIGHT < CHUNK_HEIGHT);

    initNoise();

    biomeGenerators[0] = new ForestBiome();
    biomeGenerators[1] = new SnowBiome();
    biomeGenerators[2] = new SandBiome();

    for (int i = 0; i < 3; i++)
        biomeGenerators[i]->init();

    biomeBlending = NoiseSettings(5, 0.4f, 1.7f, 0.092f);

    flaura.frequency = 100.0f;
    flaura.type = FastNoiseLite::NoiseType::NoiseType_OpenSimplex2S;
    flaura.persistance = 200.0f;
}

World::~World(){
    for (int i = 0; i< 3; i++)
        delete biomeGenerators[i];
}

void World::init(){
   Chunk* base = generateChunk(Biome::Forest, Biome::NONE, 0, Direction::NONE);
   chunks.addFront(base);
   currentChunkNode = chunks.getNode(0);

   rightGeneration = {Biome::Forest, 5, 0};
   leftGeneration = {Biome::Forest, 5, 0};

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
        Chunk* right = generateChunk(rightGeneration.type, prevBiomeRight, i, Direction::RIGHT);
        Chunk* left = generateChunk(leftGeneration.type, prevBiomeLeft, -i, Direction::LEFT);

        chunks.addFront(left);
        chunks.addEnd(right);

        prevBiomeLeft = Biome::NONE;
        prevBiomeRight = Biome::NONE;
        rightGeneration.numsChunksGenerated++;
        leftGeneration.numsChunksGenerated++;
   }
}

void World::update(){
    //Update the current chunk Node and running index based on the player's position
    //If the player is X distance from the center, call the pop and append functions
    int chunkCord = (int)floor(Camera::getPosition().x / CHUNK_WIDTH); 
    chunkCord -= chunkCord < 0 ? 1 : 0; // offset such that left chunk starts at index -1

    if (chunkCord != currentChunkIndex){
        currentChunkNode = chunks.getNodeStartingFrom(currentChunkNode, getChunkIndexInArray(currentChunkIndex), getChunkIndexInArray(chunkCord));
        currentChunkIndex = chunkCord;
    }

    if (abs(currentChunkIndex - runningIndex) >= CHUNK_POP_DISTANCE){
        Direction popDir = currentChunkIndex - runningIndex < 0 ? Direction::RIGHT : Direction::LEFT;
        Direction appendDir = popDir == Direction::LEFT ? Direction::RIGHT : Direction::LEFT;
        popChunks(popDir);
        appendChunks(appendDir);
        runningIndex += popDir == Direction::LEFT ? CHUNK_POP_SIZE : -CHUNK_POP_SIZE;
    }
}

Chunk* World::generateChunk(Biome biome, Biome previousBiome, int index, Direction direction){
     int offset = index * CHUNK_WIDTH;
     int* heightMap = biomeGenerators[biome]->getHeightMap(offset);
        
     //WORLD COORDINATES --> (0,0) is the center of the wolrd, with positive Y going up ^ and negative y going down 
     Chunk* chunk = new Chunk(biome);
     for (int i = 0; i < CHUNK_WIDTH; i++){
          
         //Zero out tiles above the heightMap
         for (int j = CHUNK_HEIGHT / 2; j > heightMap[i]; j--){
            chunk->tiles.at(abs( j - CHUNK_HEIGHT / 2 ) * CHUNK_WIDTH + i) = TileType::air;
         }

         for (int j = heightMap[i]; j > -CHUNK_HEIGHT / 2; j--){
             TileType block;

             bool failed = true;
             if (previousBiome != Biome::NONE){
                 //Check if we're within the blending distance
                 if ( (direction == Direction::RIGHT && i <= CHUNK_BLEND_DISTANCE) || (direction == Direction::LEFT && CHUNK_WIDTH - 1 - i <= CHUNK_BLEND_DISTANCE) ){

                    setNoiseSettings(biomeBlending);
                    float blendValue = sampleNoise(offset + i, j);
                    blendValue = (blendValue + 1.0f) / 2.0f;
                    if (blendValue >= CHUNK_BLEND_CUTOFF){
                        block = biomeGenerators[previousBiome]->sampleBlock(offset + (float) i / CHUNK_WIDTH + 0.1f, abs(j - heightMap[i]) + 0.1f * i);
                        failed = false;
                    }
                 }
             }

            if (failed)
                 block = biomeGenerators[biome]->sampleBlock(offset + i, abs(j - heightMap[i]));

            if (j == heightMap[i] && block == TileType::dirt)
                 block = TileType::grass;

            chunk->tiles.at(abs(j - CHUNK_HEIGHT / 2) * CHUNK_WIDTH + i) = block;
         }
     }
     
     if (biome != Biome::Desert){

        int lastTreeTime = 0;
        for (int i = 1; i < CHUNK_WIDTH - 1; i++){
            if (chunk->tiles.at(abs(heightMap[i] - CHUNK_HEIGHT / 2) * CHUNK_WIDTH + i) == TileType::air)
                continue; 

            setNoiseSettings(flaura);
            float value = sampleNoise(i, heightMap[i]);
            value = (value + 1.0f) / 2.0f;
            if (value >= FLAURA_CUTOFF && lastTreeTime > 2){
                generateFlaura(biome, i, heightMap[i], chunk);
                lastTreeTime = 0;
            }
            lastTreeTime++;
        }

     }

     delete[] heightMap;
     return chunk;
}

void World::generateFlaura(Biome biome, int x, int y, Chunk* chunk){
    assert(x > 0 && x < CHUNK_WIDTH - 1);
    y += 1;

    //configure the correct tileTypes
    TileType trunk = biome == Biome::Forest ? TileType::forestTreeTrunk : TileType::snowTreeTrunk;
    TileType trunkLeft = biome == Biome::Forest ? TileType::forestTrunkLeft : TileType::snowTrunkRight;
    TileType trunkRight = biome == Biome::Forest ? TileType::forestTrunkRight: TileType::snowTrunkLeft;
    TileType branchLeft = biome == Biome::Forest ? TileType::forestBranchLeft: TileType::snowBranchLeft;
    TileType branchRight = biome == Biome::Forest ? TileType::forestBranchRight : TileType::snowBranchRight;

    int height = rand() % (MAX_TREE_HEIGHT + 1) + MIN_TREE_HEIGHT;

    if (rand() % 11 < 5){
        if (chunk->tiles.at(abs(y - 1 - CHUNK_HEIGHT / 2) * CHUNK_WIDTH + x - 1) != TileType::air)
            chunk->tiles.at(abs(y - CHUNK_HEIGHT / 2) * CHUNK_WIDTH + x - 1) = trunkRight;
    }

    if (rand() % 11 < 5){
        if (chunk->tiles.at(abs(y - 1 - CHUNK_HEIGHT / 2) * CHUNK_WIDTH + x + 1) != TileType::air)
            chunk->tiles.at(abs(y - CHUNK_HEIGHT / 2) * CHUNK_WIDTH + x + 1) = trunkLeft;
    }

    const int minBranchHeight = 3;
    const int maxBranches = 3;
    int numBranches = 0;
    for (int i = 0; i < height; i++){
        chunk->tiles.at(abs(y + i - CHUNK_HEIGHT / 2) * CHUNK_WIDTH + x) = trunk;

        if (i >= minBranchHeight && numBranches < maxBranches){
            if (rand() % 11 < 5){
                chunk->tiles.at(abs(y + i - CHUNK_HEIGHT / 2) * CHUNK_WIDTH + x - 1) = branchLeft;
                numBranches++;
            }

            if (rand() % 11 < 5){
                chunk->tiles.at(abs(y + i - CHUNK_HEIGHT / 2) * CHUNK_WIDTH + x + 1) = branchRight;
                numBranches++;
            }

        }
    }

    if (biome == Biome::Forest){
        chunk->tiles.at(abs(y + height + 2 - CHUNK_HEIGHT / 2) * CHUNK_WIDTH + x - 1) = TileType::forestTreeTopTL;
        chunk->tiles.at(abs(y + height + 2 - CHUNK_HEIGHT / 2) * CHUNK_WIDTH + x + 0) = TileType::forestTreeTopTM;
        chunk->tiles.at(abs(y + height + 2 - CHUNK_HEIGHT / 2) * CHUNK_WIDTH + x + 1) = TileType::forestTreeTopTR;

        chunk->tiles.at(abs(y + height + 1 - CHUNK_HEIGHT / 2) * CHUNK_WIDTH + x - 1) = TileType::forestTreeTopML;
        chunk->tiles.at(abs(y + height + 1 - CHUNK_HEIGHT / 2) * CHUNK_WIDTH + x + 0) = TileType::forestTreeTopMM;
        chunk->tiles.at(abs(y + height + 1 - CHUNK_HEIGHT / 2) * CHUNK_WIDTH + x + 1) = TileType::forestTreeTopMR;

        chunk->tiles.at(abs(y + height + 0 - CHUNK_HEIGHT / 2) * CHUNK_WIDTH + x - 1) = TileType::forestTreeTopBL;
        chunk->tiles.at(abs(y + height + 0 - CHUNK_HEIGHT / 2) * CHUNK_WIDTH + x + 0) = TileType::forestTreeTopBM;
        chunk->tiles.at(abs(y + height + 0 - CHUNK_HEIGHT / 2) * CHUNK_WIDTH + x + 1) = TileType::forestTreeTopBR;
    }
    else
       chunk->tiles.at(abs(y + height - CHUNK_HEIGHT / 2) * CHUNK_WIDTH + x) = TileType::snowTreeTop;

}

void World::popChunks(Direction direction){
    for (int i = 0; i < CHUNK_POP_SIZE; i++){
        if  (direction == Direction::LEFT){
            chunks.popOffFront();
        }
        else{
            chunks.popOffEnd();
        }
    }

}

void World::appendChunks(Direction direction){
    Biome prevBiome = Biome::NONE;
    for (int i = 0; i < CHUNK_POP_SIZE; i++){
        if  (direction == Direction::LEFT){
            Chunk* c = generateChunk(leftGeneration.type, prevBiome, runningIndex - CHUNK_POP_DISTANCE - i, direction);
            chunks.addFront(c);
            leftGeneration.numsChunksGenerated += 1;

            if (leftGeneration.numsChunksGenerated >= leftGeneration.biomeLengthChunks){
                prevBiome = leftGeneration.type;
                generateNewBiomeData(leftGeneration);
            }
            else  prevBiome = Biome::NONE;
        }
        else{
            Chunk* c = generateChunk(rightGeneration.type, prevBiome, runningIndex + CHUNK_POP_DISTANCE + i, direction);
            chunks.addEnd(c);
            rightGeneration.numsChunksGenerated += 1;

            if (rightGeneration.numsChunksGenerated >= rightGeneration.biomeLengthChunks){
                prevBiome = rightGeneration.type;
                generateNewBiomeData(rightGeneration);
            }
            else  prevBiome = Biome::NONE;
        }

    }
}

void World::generateNewBiomeData(BiomeGenData& data){
     srand(time(NULL));
     int length = rand() % 10;
     data.biomeLengthChunks = length + MIN_BIOME_LENGTH;
     data.numsChunksGenerated = 0;
     data.type = static_cast<Biome>(rand() % 3);
}

TileType World::tileTypeAtWorldCoordinate(int x, int y){
    int chunkCord = x / CHUNK_WIDTH;
    int index = getChunkIndexInArray(x < 0 ? chunkCord - 1 : chunkCord);

    assert(index >= 0 && index < chunks.size());

    x -= chunkCord * CHUNK_WIDTH;
    x += x < 0 ? CHUNK_WIDTH : 0;
    y = abs(y - CHUNK_HEIGHT / 2);

    assert(y < CHUNK_HEIGHT && y >= 0 && x < CHUNK_WIDTH && x >= 0);

    int currentChunkInd = getChunkIndexInArray(currentChunkIndex);
    auto& chunk = chunks.getNodeStartingFrom(currentChunkNode, currentChunkInd, index)->value->tiles;
    return chunk[y * CHUNK_WIDTH + x];
}

void World::removeTileAtWorldCoordinate(int x, int y){
    int chunkCord = x / CHUNK_WIDTH;
    int index = getChunkIndexInArray(x < 0 ? chunkCord - 1 : chunkCord);

    assert(index >= 0 && index < chunks.size());

    x -= chunkCord * CHUNK_WIDTH;
    x += x < 0 ? CHUNK_WIDTH : 0;
    y = abs(y - CHUNK_HEIGHT / 2);

    assert(y < CHUNK_HEIGHT && y >= 0 && x < CHUNK_WIDTH && x >= 0);

    int currentChunkInd = getChunkIndexInArray(currentChunkIndex);
    auto& chunk = chunks.getNodeStartingFrom(currentChunkNode, currentChunkInd, index)->value->tiles;

    chunk[y * CHUNK_WIDTH + x] = TileType::air;
}

void World::placeTileAtWorldCoordinate(int x, int y, TileType tile){
    int chunkCord = x / CHUNK_WIDTH;
    int index = getChunkIndexInArray(x < 0 ? chunkCord - 1 : chunkCord);

    assert(index >= 0 && index < chunks.size());

    x -= chunkCord * CHUNK_WIDTH;
    x += x < 0 ? CHUNK_WIDTH : 0;
    y = abs(y - CHUNK_HEIGHT / 2);

    assert(y < CHUNK_HEIGHT && y >= 0 && x < CHUNK_WIDTH && x >= 0);

    int currentChunkInd = getChunkIndexInArray(currentChunkIndex);
    auto& chunk = chunks.getNodeStartingFrom(currentChunkNode, currentChunkInd, index)->value->tiles;

    chunk[y * CHUNK_WIDTH + x] = tile;
}

Node<Chunk>* World::getChunkAtXCoordinate(int x){
    int index = x / CHUNK_WIDTH;
    //Some shifts
    if (x < 0 && x % CHUNK_WIDTH == 0)
        index += 1;
    if (x < 0)
        index -= 1;
    
    int indexInArray = getChunkIndexInArray(index);
    int currentChunkIndexInArray = getChunkIndexInArray(currentChunkIndex);
    return chunks.getNodeStartingFrom(currentChunkNode,currentChunkIndexInArray, indexInArray);
}

inline int World::getChunkIndexInArray(int worldChunkIndex){
    //Running index = world chunk index of the center of the list
    //Distance from the center + the center
    return (worldChunkIndex - runningIndex) + (MAX_CHUNKS - 1) / 2;
}

