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
    instance->instanceRenderer->render();
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