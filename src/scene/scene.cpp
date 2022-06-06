#include "scene/scene.h"
#include "components/component.h"
#include "scene/entity.h"
#include "renderer/renderer.h"
#include "core/input.h"
#include "scripts/Animation.h"
#include "constants.h"

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
   Ref<Texture> tex = AssetPool::getTexture("/Users/shauntemellor/Documents/Documents - Shaunte’s MacBook Pro/comsci/Projects/Terraria/assets/img/LALA.png");
   s.addComponent<Component::SpriteRenderer>(SpriteSheet(tex, 1, 1));
   auto& trans = s.getComponent<Component::Transform>();
   trans.position = glm::vec2(0, 0);
   trans.scale = glm::vec2(CAM_WIDTH, CAM_HEIGHT);
   Renderer::submit(s);




   Entity& sprite = createEntity("hi");
   auto& transs = sprite.getComponent<Component::Transform>();
   transs.scale = glm::vec2(30, 30);
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

