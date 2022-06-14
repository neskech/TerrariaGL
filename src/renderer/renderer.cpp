#include "renderer/renderer.h"

#define VERTEX_SIZE 4
#define ATLAS_DIMENSIONS 512
#define SPRITE_DIMENSIONS 128
#define NUM_CELLS 4

Renderer* Renderer::instance = nullptr;

Renderer::Renderer()
{
    if (instance != nullptr)
        assert(false);
    instance = this;

}

Renderer::~Renderer(){
    //reverse order destruction
    delete debugRenderer;
    delete tileMapRenderer;
}

void Renderer::init(World* world){
    //must be called after window initialization
    spriteRenderers.reserve(MAX_INSTANCE_RENDERERS);

    tileMapRenderer = new TileMapRenderer(world);
    tileMapRenderer->init();

    debugRenderer = new DebugRenderer();
    debugRenderer->init();


}

void Renderer::render(){
    for (auto& rend : spriteRenderers){
         rend->render();
    }
    tileMapRenderer->render();
    debugRenderer->render();
}

void Renderer::submit(Terra::Entity& ent){
    auto& spr = ent.getComponent<Component::SpriteRenderer>();
    auto& trans = ent.getComponent<Component::Transform>();

    uint32_t numSprites = 0;
    for (auto& rend : spriteRenderers){
        numSprites += rend->getNumSprites();
        if (rend->hasRoomSprites() && (rend->hasRoomTextures() || rend->containsTexture(spr))
        && rend->getZIndex() == trans.zIndex){

            rend->addEntity(ent);
            return;
        }
    }

    assert(numSprites < MAX_INSTANCES);
    spriteRenderers.push_back(std::make_unique<SpriteRenderer>(*this, trans.zIndex));
    spriteRenderers.back()->init();
    spriteRenderers.back()->addEntity(ent);
    assert(spriteRenderers.size() < MAX_INSTANCE_RENDERERS);
 
    std::sort(spriteRenderers.begin(), spriteRenderers.end(), 
        [](const Scoped<SpriteRenderer>& spr1, const Scoped<SpriteRenderer>& spr2){
            return spr1->getZIndex() < spr2->getZIndex();
        }
    );
}

void Renderer::remove(Terra::Entity& ent){
    //assert that the renderer actually contains the given entity
     for (auto& rend : spriteRenderers){
         if (rend->containsEntity(ent)){ 
            rend->removeEntity(ent);
            return;
         }
    }

    assert(false);
}

