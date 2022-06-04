#pragma once
#include "util/linkedList.h"
#include "world/Biome.h"
#include "world/noise.h"
#include "constants.h"

struct Chunk{
    Biome biomeType;
    uint8_t tiles[CHUNK_WIDTH * CHUNK_HEIGHT];

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
        void update(); //checks if chunks need to be popped

        inline LinkedList<Chunk>& getChunksList(){ return chunks; }
        inline Node<Chunk>* getCurrentChunkNode(){ return currentChunkNode; }
        inline int getCurrentChunkIndex(){ return currentChunkIndex; }
        inline int getCurrentRunningIndex(){ return runningIndex; }
        inline int getChunkIndexInArray(int worldChunkIndex);
    private:
        Chunk* generateChunk(Biome biome, Biome previousBiome, int index, Direction direction);
        void popChunks();
        void appendChunks();
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

        World* instance;

};