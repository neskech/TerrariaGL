#include "renderer/tileMapRenderer.h"
#include "scene/camera.h"
#include "util/assetPool.h"

#define VERTEX_SIZE 4
#define ATLAS_DIMENSIONS 512
#define SPRITE_DIMENSIONS 128
#define NUM_CELLS 4

TileMapRenderer* TileMapRenderer::instance = nullptr;

TileMapRenderer::TileMapRenderer(World* world_): world(world_), VBO(BufferType::VBO), EBO(BufferType::EBO), VAO(), numWorkingTiles(0)
{
    if (instance != nullptr)
        assert(false);
    instance = this;

}

TileMapRenderer::~TileMapRenderer(){

}

void TileMapRenderer::init(){
    VAO.bind();
    VBO.bind();
    EBO.bind();
    //should never go out of bounds
    VBO.allocateData<float>((Camera::getCamViewWidth() + 1) * (Camera::getCamViewHeight() + 2) * 4 * VERTEX_SIZE, GL_DYNAMIC_DRAW);
    EBO.allocateData<unsigned int>((Camera::getCamViewWidth() + 1) * (Camera::getCamViewHeight() + 2) * 6, GL_STATIC_DRAW);
    addIndexData();

    VAO.addAtribute(2, GL_FLOAT, sizeof(float), sizeof(float) * VERTEX_SIZE);
    VAO.addAtribute(2, GL_FLOAT, sizeof(float), sizeof(float) * VERTEX_SIZE);

    VAO.unBind();
    VBO.unBind();
    EBO.unBind();

    shader = AssetPool::getShader("/Users/shauntemellor/Documents/Documents - Shaunte’s MacBook Pro/comsci/Projects/Terraria/assets/shaders/worldShader.glsl");
    TexParams params{
        .behaviorX = GL_CLAMP_TO_EDGE,
        .behaviorY = GL_CLAMP_TO_EDGE,
        .magFilter = GL_LINEAR,
        .minFilter = GL_LINEAR_MIPMAP_LINEAR,
        .mipMapLevels = 1
    };
    tileAtlas = AssetPool::getTexture("/Users/shauntemellor/Documents/Documents - Shaunte’s MacBook Pro/comsci/Projects/Terraria/assets/img/SpriteSheet.png", params);
    
}

void TileMapRenderer::render(){
    updateWorldData();
    renderWorld();
}

void TileMapRenderer::renderWorld(){
    VAO.bind();
    VBO.bind();

    shader->activate();
    shader->setVec2("sprite_dims", glm::vec2(SPRITE_DIMENSIONS, SPRITE_DIMENSIONS));
    shader->setFloat("cols", NUM_CELLS);
    shader->setmat4("ortho", Camera::getProjectionMatrix());
    shader->setmat4("view", Camera::getViewMatrix());

    glActiveTexture(GL_TEXTURE0);
    tileAtlas->bind();
    shader->uploadTexture("texture_atlas", 0);

    EBO.bind();
    glDrawElements(GL_TRIANGLES, numWorkingTiles * 6, GL_UNSIGNED_INT, 0);
    shader->deActivate();

    tileAtlas->unBind();
    VBO.unBind();
    VAO.unBind();
    numWorkingTiles = 0;
}

void TileMapRenderer::updateWorldData(){
      const glm::vec2& cameraPos = Camera::getPosition();

      //shift the coordinates to the top left of our view
      int x_cord = (int) floor(cameraPos.x - CAM_WIDTH / 2.0f);
      int y_cord = (int) floor(cameraPos.y + CAM_HEIGHT / 2.0f);

      //Usually the left most tile will not be shown by the camera, so shift the pos.x over by 1
        
      //CAM_WIDTH + 2 so we can capture the leftmost and rightmost tiles of our view
      for (int i = 0; i < CAM_WIDTH + 1; i++){

          //Find the chunk index in the  and in the array. Negative chunks should start at index -1, while positive start at 0, so add a shift of -1
          int chunkIndex = x_cord / CHUNK_WIDTH;
          if (x_cord < 0 && x_cord % CHUNK_WIDTH == 0)
              chunkIndex += 1;

          //Find the chunk-relative index of the x component. Left = 0, right = CHUNK_WIDTH - 1. As such, negative indices must be made positive and inverted
          int tileIndex_x = x_cord - chunkIndex * CHUNK_WIDTH; 
          tileIndex_x += tileIndex_x < 0 ? CHUNK_WIDTH : 0;
         //tileIndex_x += tileIndex_x < 0 ? CHUNK_WIDTH : 0;

          // coordinates have (0.0) as its center, thus the y index requires a shift from the y cord
          int tileIndex_y = abs(y_cord - CHUNK_HEIGHT / 2); 
      
         //fetch the current chunk to read data from
          Node<Chunk>* chunk = world->getChunkAtXCoordinate(x_cord);
          auto& currentChunk = chunk->value->tiles;
          for (int y = 0; y <= CAM_HEIGHT; y++){

               TileType texID = currentChunk.at((y + tileIndex_y) * CHUNK_WIDTH + tileIndex_x);
               if (texID == TileType::air)
                    continue;

               addTileData(numWorkingTiles, x_cord, y_cord - y, (int) texID - 1);
               numWorkingTiles++;
          }

          //Advance the x coordinate by 1 unit tile
          x_cord += 1; 
      }
}

void TileMapRenderer::addTileData(int index, int x, int y, int texID){
    VBO.bind();
    float* buffer_ptr = VBO.mapBuffer<float>();

    int offset = index * 4 * VERTEX_SIZE;
    for (int i = 0; i < 4; i++){
        int row = i < 2;
        int col = i == 1 || i == 3;

        buffer_ptr[0 + i * VERTEX_SIZE + offset] = x + col;
        buffer_ptr[1 + i * VERTEX_SIZE + offset] = y - row;
        buffer_ptr[2 + i * VERTEX_SIZE + offset] = ((texID % NUM_CELLS) * SPRITE_DIMENSIONS + SPRITE_DIMENSIONS * col) / (float)ATLAS_DIMENSIONS;
        buffer_ptr[3 + i * VERTEX_SIZE + offset] = 1.0f - ( ((texID / NUM_CELLS) * SPRITE_DIMENSIONS + SPRITE_DIMENSIONS * row) / (float)ATLAS_DIMENSIONS );
    }
    VBO.unMapBuffer();
    VBO.unBind();
}

void TileMapRenderer::addIndexData(){
    EBO.bind();
    unsigned int* buffer_ptr = EBO.mapBuffer<unsigned int>();

    int numQuads = (Camera::getCamViewWidth() + 1) * (Camera::getCamViewHeight() + 2);
    for (int i = 0; i < numQuads; i++){
        buffer_ptr[0 + i * 6] = 0 + i * 4;
        buffer_ptr[1 + i * 6] = 1 + i * 4;
        buffer_ptr[2 + i * 6] = 3 + i * 4;
        buffer_ptr[3 + i * 6] = 0 + i * 4;
        buffer_ptr[4 + i * 6] = 3 + i * 4;
        buffer_ptr[5 + i * 6] = 2 + i * 4;
    }

    EBO.unMapBuffer();
    EBO.unBind();
}