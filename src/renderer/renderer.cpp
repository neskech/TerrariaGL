#include "renderer/renderer.h"

Renderer* Renderer::instance = nullptr;

Renderer::Renderer(){
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
}

void Renderer::render(){
    for (int i = 0; i < instance->numBatches; i++){
        instance->instanceBatches[i].render();
    }
}

void Renderer::submit(Component::SpriteRenderer& spr){
    assert(instance->instanceRenderer->numSprites <= MAX_INSTANCES);
    instance->instanceBatches[instance->numBatches].addSpriteRenderer(spr);
}

void Renderer::remove(Component::SpriteRenderer& spr){
    //assert that the renderer actually contains the given Sprite
    assert(instance->instanceRenderer->renderers[spr.index] == &spr);
    instance->instanceRenderer->removeSpriteRenderer(spr);

}