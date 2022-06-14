#include "world/world.h"
#include "scene/camera.h"
#include "constants.h"


World* World::instance = nullptr;


World::World(): biomeBlending(true), currentChunkIndex(0), runningIndex(0){
    if (instance != nullptr)
        assert(false);
    instance = this;

    biomeGenerators[0] = new ForestBiome();
    biomeGenerators[1] = new SandBiome();
    biomeGenerators[2] = new SnowBiome();

    for (int i = 0; i < 3; i++)
        biomeGenerators[i]->init();

    biomeBlending.frequency = 1000.5f;
    biomeBlending.octaves = 8;
    biomeBlending.persistance = 500.2f;
    biomeBlending.lacrinarity = 1000.0f;
    biomeBlending.type = FastNoiseLite::NoiseType::NoiseType_OpenSimplex2S;
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
        std::cout<<"YOU OUTTA RANGE BUDDY!! " << chunkCord << " " << runningIndex << "\n";
        Direction popDir = currentChunkIndex - runningIndex < 0 ? Direction::RIGHT : Direction::LEFT;
        Direction appendDir = popDir == Direction::LEFT ? Direction::RIGHT : Direction::LEFT;
        popChunks(popDir);
        appendChunks(appendDir);
        runningIndex += popDir == Direction::LEFT ? CHUNK_POP_SIZE : -CHUNK_POP_SIZE;
        std::cout << "NEW SIZE " << chunks.size() << std::endl;
    }
}

Chunk* World::generateChunk(Biome biome, Biome previousBiome, int index, Direction direction){
     int offset = index * CHUNK_WIDTH;
     int* heightMap = biomeGenerators[biome]->getHeightMap(offset);
        
     //WORLD COORDINATES --> (0,0) is the center of the wolrd, with positive Y going up ^ and negative y going down 
     Chunk* chunk = new Chunk(biome);
     for (int i = 0; i < CHUNK_WIDTH; i++){
          
         //Zero out tiles above the heightMap
         for (int j = CHUNK_HEIGHT / 2; j > heightMap[i]; j--)
            chunk->tiles.at(abs( j - CHUNK_HEIGHT / 2 ) * CHUNK_WIDTH + i) = TileType::air;

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
                        block = biomeGenerators[previousBiome]->sampleBlock(offset + (float) i / CHUNK_WIDTH + 0.1f, abs(j) + 0.1f * i);
                        failed = false;
                    }
                 }
             }

             if (failed)
                 block = biomeGenerators[biome]->sampleBlock(offset + i, abs(j));

            if (j == heightMap[i] && block == TileType::dirt)
                block = TileType::grass;
            
            chunk->tiles.at(abs(j - CHUNK_HEIGHT / 2) * CHUNK_WIDTH + i) = block;
 
         }
     }

     delete[] heightMap;
     return chunk;
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

