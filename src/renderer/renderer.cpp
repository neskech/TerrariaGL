#include "renderer/renderer.h"

#define VERTEX_SIZE 4
#define ATLAS_DIMENSIONS 512
#define SPRITE_DIMENSIONS 64
#define NUM_CELLS 8

Renderer* Renderer::instance = nullptr;

Renderer::Renderer(World* world_): world(world_), worldVBO(BufferType::VBO), worldEBO(BufferType::EBO), worldVAO(sizeof(float) * VERTEX_SIZE)
{
    if (instance != nullptr)
        assert(false);
    instance = this;

}

Renderer::~Renderer(){
    delete instanceRenderer;
}

void Renderer::init(){
    //must be called after window initialization
    instance->instanceRenderer = new InstanceRenderer();
    instance->instanceRenderer->init();

    instance->worldVAO.bind();
    instance->worldVBO.bind();
    instance->worldEBO.bind();
    //should never go out of bounds
    instance->worldVBO.allocateData<float>((Camera::getCamViewWidth() + 1) * (Camera::getCamViewHeight() + 2) * 4 * VERTEX_SIZE);
    instance->worldEBO.allocateData<unsigned int>((Camera::getCamViewWidth() + 1) * (Camera::getCamViewHeight() + 2) * 6);
    addIndexData();

    instance->worldVAO.addAtribute(2, GL_FLOAT, sizeof(float));
    instance->worldVAO.addAtribute(2, GL_FLOAT, sizeof(float));

    instance->worldVAO.unBind();
    instance->worldVBO.unBind();
    instance->worldEBO.unBind();

    instance->worldShader = AssetPool::getShader("/Users/shauntemellor/Documents/Documents - Shaunte’s MacBook Pro/comsci/Projects/Terraria/assets/shaders/worldShader.glsl");
    instance->tileAtlas = AssetPool::getTexture("/Users/shauntemellor/Documents/Documents - Shaunte’s MacBook Pro/comsci/Projects/Terraria/assets/img/SpriteSheet.png");
    
}

void Renderer::render(){
    instance->instanceRenderer->render();
    updateWorldData();
    renderWorld();


}

void Renderer::submit(Terra::Entity& ent){
    assert(instance->instanceRenderer->numSprites <= MAX_INSTANCES);
    instance->instanceRenderer->addEntity(ent);
}

void Renderer::remove(Terra::Entity& ent){
    //assert that the renderer actually contains the given entite
    Component::SpriteRenderer& spr = ent.getComponent<Component::SpriteRenderer>();
    assert(instance->instanceRenderer->index_map.find(&ent) != instance->instanceRenderer->index_map.end());
    instance->instanceRenderer->removeEntity(ent);
}

void Renderer::renderWorld(){
    instance->worldVAO.bind();
    instance->worldVBO.bind();

    instance->worldShader->activate();
    instance->worldShader->setVec2("sprite_dims", glm::vec2(SPRITE_DIMENSIONS, SPRITE_DIMENSIONS));
    instance->worldShader->setFloat("cols", NUM_CELLS);
    instance->worldShader->setmat4("ortho", Camera::getProjectionMatrix());
    instance->worldShader->setmat4("view", Camera::getViewMatrix());

    glActiveTexture(GL_TEXTURE0);
    instance->tileAtlas->bind();
    instance->worldShader->uploadTexture("texture_atlas", 0);

    instance->worldEBO.bind();
    glDrawElements(GL_TRIANGLES, instance->numWorkingTiles * 6, GL_UNSIGNED_INT, 0);
    instance->worldShader->deActivate();

    instance->tileAtlas->unBind();
    instance->worldVBO.unBind();
    instance->worldVAO.unBind();
    instance->numWorkingTiles = 0;
}

void Renderer::updateWorldData(){
      const int CAM_WIDTH = Camera::getCamViewWidth();
      const int CAM_HEIGHT = Camera::getCamViewHeight();

      const glm::vec2& cameraPos = Camera::getPosition();

      //shift the coordinates to the top left of our view
      int x_cord = (int) floor(cameraPos.x - CAM_WIDTH / 2);
      int y_cord = (int) floor(cameraPos.y + CAM_HEIGHT / 2);

      //Usually the left most tile will not be shown by the camera, so shift the pos.x over by 1

      //Get the current chunk node and its index in the chunk array
      Node<Chunk>* node = instance->world->getCurrentChunkNode();
      int nodeIndex = instance->world->getChunkIndexInArray(instance->world->getCurrentChunkIndex());
        
      //CAM_WIDTH + 2 so we can capture the leftmost and rightmost tiles of our view
      for (int i = 0; i < CAM_WIDTH + 1; i++){

          //Find the chunk index in the world and in the array. Negative chunks should start at index -1, while positive start at 0, so add a shift of -1
          int chunkIndex = x_cord / CHUNK_WIDTH;
          int chunkIndexArray = instance->world->getChunkIndexInArray(chunkIndex < 0 ? chunkIndex - 1 : chunkIndex);

          //Find the chunk-relative index of the x component. Left = 0, right = CHUNK_WIDTH - 1. As such, negative indices must be made positive and inverted
          int tileIndex_x = x_cord - chunkIndex * CHUNK_WIDTH; 
          tileIndex_x += tileIndex_x < 0 ? CHUNK_WIDTH : 0;

          //World coordinates have (0.0) as its center, thus the y index requires a shift from the y cord
          int tileIndex_y = abs(y_cord - CHUNK_HEIGHT / 2); 
          
      
          //fetch the current chunk to read data from
          auto& currentChunk = instance->world->getChunksList().getNodeStartingFrom(node, nodeIndex, chunkIndexArray)->value->tiles;
          for (int y = 0; y <= CAM_HEIGHT; y++){

            //    if (y + tileIndex_y >= CHUNK_HEIGHT) {
            //        std::cout<<"OUT OF BOUNDS!\n";
            //        break;
            //    }

               BlockType texID = currentChunk.at((y + tileIndex_y) * CHUNK_WIDTH + tileIndex_x);
               if (texID == BlockType::air)
                    continue;

               addTileData(instance->numWorkingTiles, x_cord, y_cord - y, (int) texID - 1);
               instance->numWorkingTiles++;
          }

          //Advance the x coordinate by 1 unit tile
          x_cord += 1; 
      }
}

void Renderer::addTileData(int index, int x, int y, int texID){
    instance->worldVBO.bind();
    float* buffer_ptr = instance->worldVBO.mapBuffer<float>();

    int offset = index * 4 * VERTEX_SIZE;
    for (int i = 0; i < 4; i++){
        int row = i < 2;
        int col = i == 1 || i == 3;

        buffer_ptr[0 + i * VERTEX_SIZE + offset] = x + col;
        buffer_ptr[1 + i * VERTEX_SIZE + offset] = y + row;
        buffer_ptr[2 + i * VERTEX_SIZE + offset] = ((texID % NUM_CELLS) * SPRITE_DIMENSIONS + SPRITE_DIMENSIONS * col) / (float)ATLAS_DIMENSIONS;
        buffer_ptr[3 + i * VERTEX_SIZE + offset] = 1.0f - ( ((texID / NUM_CELLS) * SPRITE_DIMENSIONS + SPRITE_DIMENSIONS * row) / (float)ATLAS_DIMENSIONS );
    }

    instance->worldVBO.unMapBuffer();
    instance->worldVBO.unBind();
}

void Renderer::addIndexData(){
    instance->worldEBO.bind();
    unsigned int* buffer_ptr = instance->worldEBO.mapBuffer<unsigned int>();

    int numQuads = (Camera::getCamViewWidth() + 1) * (Camera::getCamViewHeight() + 2);
    for (int i = 0; i < numQuads; i++){
        buffer_ptr[0 + i * 6] = 0 + i * 4;
        buffer_ptr[1 + i * 6] = 1 + i * 4;
        buffer_ptr[2 + i * 6] = 3 + i * 4;
        buffer_ptr[3 + i * 6] = 0 + i * 4;
        buffer_ptr[4 + i * 6] = 3 + i * 4;
        buffer_ptr[5 + i * 6] = 2 + i * 4;
    }

    instance->worldEBO.unMapBuffer();
    instance->worldEBO.unBind();
}