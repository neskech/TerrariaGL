#include "scene/scene.h"
#include "components/component.h"


Scene::Scene(){
    renderer = new Renderer();
}

Scene::~Scene(){
    delete renderer;
}

void Scene::init(){
    registry.on_construct<Component::SpriteRenderer>().connect<&Renderer::addToBatch>(renderer);
    Terra::Entity e = createEntity();
    //e.addComponent<Component::SpriteRenderer>();
}

void Scene::update(float timeStep){
    auto view = registry.view<Component::AnimationController>();
    for (auto entity : view){
        auto& animationController = view.get<Component::AnimationController>(entity);
        animationController.advance(timeStep);
    }

}

void Scene::render(){
    Renderer::render();
}

void Scene::addToRenderer(Terra::Entity& ent){
    Renderer::addToBatch(ent.getComponent<Component::SpriteRenderer>());
}

Terra::Entity Scene::createEntity(){
    Terra::Entity ent(this);
    ent.addComponent<Component::Tag>(std::string("Object #") + std::to_string((int)ent.ent));
    ent.addComponent<Component::Transform>();
    return ent;
}

void Scene::deleteEntity(Terra::Entity& ent){
    if (ent.hasComponent<Component::SpriteRenderer>())
        Renderer::deleteFromBatch(ent.getComponent<Component::SpriteRenderer>());
    registry.destroy(ent.ent);
}

