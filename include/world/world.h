#pragma once
#include "util/linkedList.h"
#include "world/Biome.h"
#include "world/noise.h"
#include "constants.h"
#include "pch.h"

struct Chunk{
    Biome biomeType;
    std::array<TileType, CHUNK_WIDTH * CHUNK_HEIGHT> tiles;
    //uint8_t tiles[CHUNK_WIDTH * CHUNK_HEIGHT];

    Chunk(Biome biomeType_): biomeType(biomeType_) {}
};

struct BiomeGenData{
    Biome type;
    int biomeLengthChunks;
    int numsChunksGenerated;
};

enum class Direction{
    NONE = 0,
    LEFT,
    RIGHT,
};

class World{
    public:
        World();
        World(World& other) = delete;
        World(const World& other) = delete;
        World(World&& other) = delete;
        ~World();
        
        void init(); //Generates the first MAX_CHUNKS chunks
        void update(); //checks if chunks need to be popped  
        TileType tileTypeAtWorldCoordinate(int x, int y);
        Node<Chunk>* getChunkAtXCoordinate(int x);
        
        inline Biome getBiomeTypeAt(int x){ return getChunkAtXCoordinate(x)->value->biomeType; }
        inline LinkedList<Chunk>& getChunksList(){ return chunks; }
        inline int getChunkIndexInArray(int worldChunkIndex);

    private:
        Chunk* generateChunk(Biome biome, Biome previousBiome, int index, Direction direction);
        void popChunks(Direction direction);
        void appendChunks(Direction direction);
        void generateNewBiomeData(BiomeGenData& data);

        LinkedList<Chunk> chunks;
        BiomeRule* biomeGenerators[3];
        Node<Chunk>* currentChunkNode; //To make iteration through the linked list faster
        int currentChunkIndex; //The world chunk index of the center of the camera
        int runningIndex; //The location of the center chunk of the list

        BiomeGenData rightGeneration;
        BiomeGenData leftGeneration;
        NoiseSettings biomeBlending;

        static World* instance;

};