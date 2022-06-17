#include "scene/scene.h"
#include "components/component.h"
#include "scene/entity.h"
#include "renderer/renderer.h"
#include "core/input.h"
#include "scripts/characterController.h"
#include "scripts/backgroundManager.h"
#include "scripts/Inventory.h"
#include "constants.h"
#include "physics/physicsSystem.h"

using namespace Terra;
static Entity* controller;

Scene::Scene(){
   world = new World();
   renderer = new Renderer();
   camera = new Camera();

   entites.reserve(MAX_INSTANCES);
}

Scene::~Scene(){
    for (int i = 0; i < entites.size(); i++)
        deleteEntity(entites[i], false);

    delete world;
    delete renderer;
    delete camera;
}


void Scene::init(){
   renderer->init(world);
   world->init();

   Entity& sprite = createEntity("Player");
   sprite.addComponent<Component::Script>((ScriptableObject*)(new CharacterController(sprite, this)));
   controller = &sprite;

   Entity& bm = createEntity("BackgroundManager");
   bm.addComponent<Component::Script>((ScriptableObject*)(new BackgroundManager(bm, this)));

   Entity& inv = createEntity("InventoryManager");
   inv.addComponent<Component::Script>((ScriptableObject*)(new Inventory(inv, this)));
   
   auto view = registry.view<Component::Script>();
   for (auto entity : view){
       auto& scr = view.get<Component::Script>(entity);
       scr.start();
   }
 
}

void Scene::update(float timeStep){
    world->update();

    //Have to update this first because it moves the camera. If not updated first, everything will not be synched with the camera
    auto& script = controller->getComponent<Component::Script>();
    script.update(timeStep);

    auto view = registry.view<Component::Script>();
    for (auto entity : view){
        if (entity == controller->getInnerEntity())
            continue;

        auto& scr = view.get<Component::Script>(entity);
        scr.update(timeStep);
    }


    
    simulate(registry, world, timeStep);

    
}

void Scene::render(){
    renderer->render();
}

void Scene::addToRenderer(Entity& ent){
    renderer->submit(ent);
}

void Scene::removeFromRenderer(Entity& ent){
    renderer->remove(ent);
}

Terra::Entity& Scene::createEntity(const std::string& name){
    Terra::Entity ent(registry.create(), this);

    if (name != std::string("None"))
        ent.addComponent<Component::Tag>(std::string(name));
    else
        ent.addComponent<Component::Tag>(std::string("Object #") + std::to_string((int)ent.getInnerEntity()));

    ent.addComponent<Component::Transform>();
    entites.push_back(ent);
    return entites.back();
}

void Scene::deleteEntity(Terra::Entity& ent, bool removeFromRenderer){
    if (removeFromRenderer && ent.hasComponent<Component::SpriteRenderer>()){
        renderer->remove(ent);
    }

    if (ent.hasComponent<Component::Script>())
        ent.getComponent<Component::Script>().onDestroy();

    for (int i = 0; i < entites.size(); i++){
        if (&entites[i] == &ent){
            entites.erase(entites.begin() + i);
            break;
        }
    }
    //Bug in the library. This method causes heap corruption. Spent 5 hours trying to debug this. DO NOT USE Scene::deleteEntity()
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

Terra::Entity* Scene::getEntityByTagName(const std::string& tagName){
    for (auto& entity : entites){
        if (entity.getComponent<Component::Tag>().name == tagName)
            return &entity;  
    }
    return nullptr;
}

