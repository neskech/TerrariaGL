#include "scene/scene.h"
#include "components/component.h"
#include "scene/entity.h"
#include "renderer/renderer.h"
#include "core/input.h"
#include "scripts/Animation.h"

using namespace Terra;


Scene::Scene(){
   renderer = new Renderer();
   camera = new Camera();
}

Scene::~Scene(){
    delete renderer;
    delete camera;

    std::cout <<"About to delete these guys!\n";
    // for (auto& entity : entites)
    //     deleteEntity(entity);
     std::cout <<"About to delete these guys!\n";
}


void Scene::init(){
   renderer->init();
   
   Entity& sprite = createEntity();
   auto& trans = sprite.getComponent<Component::Transform>();
   trans.scale = glm::vec2(30, 30);
   trans.position = glm::vec2(-0.5f, -0.5f);

   sprite.addComponent<AnimationScript>(sprite);
   sprite.getComponent<AnimationScript>().start();

 
}

void Scene::update(float timeStep){
    auto view = registry.view<AnimationScript>();
    for (auto& entity : view){
        auto& ani = view.get<AnimationScript>(entity);
        ani.update(timeStep);
    }


}

void Scene::render(){
    Renderer::render();
}

void Scene::addToRenderer(Entity& ent){
    Renderer::submit(ent);
}

Terra::Entity& Scene::createEntity(const char* name){
    Terra::Entity ent(registry.create(), this);

    if (name != nullptr)
        ent.addComponent<Component::Tag>(std::string(name));
    else
        ent.addComponent<Component::Tag>(std::string("Object #") + std::to_string((int)ent.getInnerEntity()));

    ent.addComponent<Component::Transform>();
    entites.push_back(ent);
    return entites.back();
}

void Scene::deleteEntity(Terra::Entity& ent){
    if (ent.hasComponent<Component::SpriteRenderer>())
        Renderer::remove(ent);
    registry.destroy(ent.getInnerEntity());
}

void Scene::deleteEntityByTagName(const std::string& tagName){
    for (auto& entity : entites){
        if (entity.getComponent<Component::Tag>().name == tagName){
            deleteEntity(entity);
            return;
        }
    }
}

