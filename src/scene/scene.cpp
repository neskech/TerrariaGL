#include "scene/scene.h"
#include "components/component.h"
#include "scene/entity.h"
#include "renderer/renderer.h"
#include "core/input.h"
#include "scripts/Animation.h"
#include "constants.h"
#include "physics/physicsSystem.h"

using namespace Terra;


Scene::Scene(){
   world = new World();
   renderer = new Renderer(world);
   camera = new Camera();

   entites.reserve(MAX_INSTANCES);
}

Scene::~Scene(){
    delete world;
    delete renderer;
    delete camera;

}


void Scene::init(){
   renderer->init();
   world->init();
   
   Entity& s = createEntity("Background");
   Ref<Texture> tex = AssetPool::getTexture("/Users/shauntemellor/Documents/Documents - Shaunte’s MacBook Pro/comsci/Projects/Terraria/assets/img/Shotty.png");
   s.addComponent<Component::SpriteRenderer>(SpriteSheet(tex, 1, 1));
   auto& trans = s.getComponent<Component::Transform>();
   trans.position.y = 20.0f;
   trans.scale = glm::vec2(CAM_WIDTH + 2, CAM_HEIGHT + 2);
   Renderer::submit(s);



   Entity& sprite = createEntity("hi");
   sprite.addComponent<Component::physicsBody>();
   sprite.addComponent<Component::AABB>(1.0f, 1.3f);
   auto& transs = sprite.getComponent<Component::Transform>();
   transs.scale = glm::vec2(4, 4);
   Component::Script& scr = sprite.addComponent<Component::Script>((Component::ScriptableObject*)(new AnimationScript(sprite, this)));
   scr.start();
   

 
}

void Scene::update(float timeStep){
    world->update();

    auto view = registry.view<Component::Script>();
    for (auto entity : view){
        auto& scr = view.get<Component::Script>(entity);
        scr.update(timeStep);
    }
    
    simulate(registry, world,timeStep);

    
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

Terra::Entity* Scene::getEntityByTagName(const std::string& tagName){
    for (auto& entity : entites){
        if (entity.getComponent<Component::Tag>().name == tagName)
            return &entity;  
    }
    return nullptr;
}

