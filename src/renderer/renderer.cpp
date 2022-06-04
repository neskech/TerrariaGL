#include "renderer/renderer.h"

#define VERTEX_SIZE 3

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
    instance->worldVBO.allocateData<float>(Camera::getCamViewWidth() * Camera::getCamViewHeight() * 4 * VERTEX_SIZE);
    instance->worldEBO.allocateData<unsigned int>(Camera::getCamViewWidth() * Camera::getCamViewHeight() * 6);
    addIndexData();

    instance->worldVAO.addAtribute(2, GL_FLOAT, sizeof(float));
    instance->worldVAO.addAtribute(1, GL_FLOAT, sizeof(float));

    instance->worldVAO.unBind();
    instance->worldVBO.unBind();
    instance->worldEBO.unBind();

    instance->worldShader = AssetPool::getShader("/Users/shauntemellor/Documents/Documents - Shaunte’s MacBook Pro/comsci/Projects/Terraria/assets/shaders/instance->worldShader.glsl");
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
    instance->worldShader->setVec2("sprite_dims", glm::vec2(64, 64));
    instance->worldShader->setFloat("cols", 8);

    glActiveTexture(GL_TEXTURE0);
    instance->tileAtlas->bind();
    instance->worldShader->uploadTexture("texture_atlas", 0);

    instance->worldEBO.bind();
    glDrawElements(GL_TRIANGLES, Camera::getCamViewWidth() * Camera::getCamViewHeight() * 6, GL_UNSIGNED_INT, 0);
    instance->worldShader->deActivate();

    instance->tileAtlas->unBind();
    instance->worldVBO.unBind();
    instance->worldVAO.unBind();
}

void Renderer::updateWorldData(){
      //Make this a reference?
      const int CAM_WIDTH = Camera::getCamViewWidth();
      const int CAM_HEIGHT = Camera::getCamViewHeight();

      glm::vec2 cameraPos = Camera::getPosition();
      if (cameraPos.x < 0)
        cameraPos.x -= CHUNK_WIDTH; //To offset negative chunks with an index of -1

      Node<Chunk>* node = instance->world->getCurrentChunkNode();
      int nodeIndex = instance->world->getChunkIndexInArray(instance->world->getCurrentChunkIndex());
        
      for (int i = 0; i < CAM_WIDTH; i++){
          int chunkIndex = (int) (cameraPos.x / CHUNK_WIDTH);
          int chunkIndexArray = instance->world->getChunkIndexInArray(chunkIndex);

          int tileIndex_x = (int) (cameraPos.x - chunkIndex * CHUNK_WIDTH); //In range 0-CHUNK_WIDTH
          if (tileIndex_x < 0)
             tileIndex_x += CHUNK_WIDTH;

          int tileIndex_y = (int) abs(cameraPos.y - CHUNK_HEIGHT / 2); //FIX -- World coordinates have (0.0) as its center

          uint8_t* currentChunk = instance->world->getChunksList().getNodeStartingFrom(node, nodeIndex, chunkIndexArray)->value->tiles;

          for (int y = 0; y < CAM_HEIGHT; y++){
               int index = (y + tileIndex_y) * CHUNK_WIDTH + tileIndex_x;
               int texID = currentChunk[index];
               int x_cord = (int) cameraPos.x;
               int y_cord = (int) cameraPos.y - y;
               addTileData(index, x_cord, y_cord, texID);
          }
        
          cameraPos.x += 1; //The width of a single tile
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
        buffer_ptr[2 + i * VERTEX_SIZE + offset] = texID;
    }

    instance->worldVBO.unMapBuffer();
    instance->worldVBO.unBind();
}

void Renderer::addIndexData(){
    instance->worldEBO.bind();
    unsigned int* buffer_ptr = instance->worldEBO.mapBuffer<unsigned int>();

    int numQuads = Camera::getCamViewWidth() * Camera::getCamViewHeight();
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