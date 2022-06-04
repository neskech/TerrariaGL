#pragma once
#include "util/linkedList.h"
#include "world/Biome.h"
#include "world/noise.h"
#include "constants.h"
#include <array>


static constexpr int MAX_CHUNKS = 11; //5 on each side, 1 in the middle
static constexpr int CHUNK_POP_SIZE = 3; //Amount of chunks to pop off one side and add to the other
static constexpr int POP_DISTANCE = (MAX_CHUNKS - 1) / 2; //Distance from the center at which to start popping chunks
static constexpr int MIN_BIOME_LENGTH = 6;


struct Chunk{
    Biome biomeType;
    std::array<BlockType, CHUNK_WIDTH * CHUNK_HEIGHT> tiles;
    //uint8_t tiles[CHUNK_WIDTH * CHUNK_HEIGHT];

    Chunk(Biome biomeType_): biomeType(biomeType_) {}
};

struct BiomeGenData{
    Biome type;
    int biomeLengthChunks;
    int numsChunksGenerated;
};

enum class Direction{
    LEFT,
    RIGHT,
    NONE,
};

class World{
    public:
        World();
        ~World();
        
        void init(); //Generates the first MAX_CHUNKS chunks
        void update(float playerPosX); //checks if chunks need to be popped

        inline LinkedList<Chunk>& getChunksList(){ return chunks; }
        inline Node<Chunk>* getCurrentChunkNode(){ return currentChunkNode; }
        inline int getCurrentChunkIndex(){ return currentChunkIndex; }
        inline int getCurrentRunningIndex(){ return runningIndex; }

        inline int getChunkIndexInArray(int worldChunkIndex){
            //Running index = world chunk index of the center of the list
            //Distance from the center + the center
            return (worldChunkIndex - runningIndex) + (MAX_CHUNKS - 1) / 2;
        }

    private:
        Chunk* generateChunk(Biome biome, Biome previousBiome, int index, Direction direction);
        void popChunks(Direction direction);
        void appendChunks(Direction direction);
        void generateNewBiomeData(BiomeGenData& data);

        LinkedList<Chunk> chunks;
        BiomeRule* biomeGenerators[3];
        Node<Chunk>* currentChunkNode; //To make iteration through the linked list faster
        int currentChunkIndex;
        int runningIndex; //The location of the center chunk of the list

        BiomeGenData rightGeneration;
        BiomeGenData leftGeneration;
        NoiseSettings biomeBlending;
        const int blendDistance = 10;
        const float blendCutoffValue = 0.8f;

        static World* instance;

};