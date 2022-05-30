#include "scene/scene.h"
#include "components/component.h"
#include "scene/entity.h"
#include "renderer/renderer.h"
#include "core/input.h"

using namespace Terra;


Scene::Scene(){
   renderer = new Renderer();
   camera = new Camera(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
}

Scene::~Scene(){
    delete renderer;
    delete camera;
}

void Scene::init(){
   renderer->init();
   
   Entity sprite = createEntity();
   auto& trans = sprite.getComponent<Component::Transform>();
   trans.scale = glm::vec2(30, 30);
   trans.position = glm::vec2(-0.5f, -0.5f);
   trans.rotation.x = glm::radians(45.0f);

   Component::SpriteRenderer& spr = sprite.addComponent<Component::SpriteRenderer>(sprite, SpriteSheet{"/Users/shauntemellor/Documents/Documents - Shaunte’s MacBook Pro/comsci/Projects/Terraria/assets/img/kanan.png"});
   Renderer::addToBatch(spr);

    Entity spritee = createEntity();
   auto& transs = spritee.getComponent<Component::Transform>();
   transs.scale = glm::vec2(20, 30);
   transs.position = glm::vec2(20.9f, -30.3f);

   Component::SpriteRenderer& sprr = spritee.addComponent<Component::SpriteRenderer>(spritee, SpriteSheet{"/Users/shauntemellor/Documents/Documents - Shaunte’s MacBook Pro/comsci/Projects/Terraria/assets/img/kanan.png"});
   Renderer::addToBatch(sprr);
}

void Scene::update(float timeStep){
    auto view = registry.view<Component::AnimationController>();
    for (auto entity : view){
        auto& animationController = view.get<Component::AnimationController>(entity);
        animationController.advance(timeStep);
    }

    float speed = 80.0f * timeStep;
    if (KeyListener::isKeyPressed(GLFW_KEY_S))
        Camera::changePosition(camera->getPosition() + glm::vec3(0.0f, 1.0f, 0.0f) * speed);
    else if (KeyListener::isKeyPressed(GLFW_KEY_W))
        Camera::changePosition(camera->getPosition() - glm::vec3(0.0f, 1.0f, 0.0f)* speed);
    else if (KeyListener::isKeyPressed(GLFW_KEY_A))
         Camera::changePosition(camera->getPosition() + glm::vec3(1.0f, 0.0f, 0.0f) * speed);
    else if (KeyListener::isKeyPressed(GLFW_KEY_D))
         Camera::changePosition(camera->getPosition() - glm::vec3(1.0f, 0.0f, 0.0f) * speed);

}

void Scene::render(){
    Renderer::render();
}

void Scene::addToRenderer(entt::entity ent){
    //Renderer::addToBatch(ent.getComponent<Component::SpriteRenderer>());
}

Terra::Entity Scene::createEntity(){
    Terra::Entity ent(registry.create(), this);
    ent.addComponent<Component::Tag>(std::string("Object #") + std::to_string((int)ent.getInnerEntity()));
    ent.addComponent<Component::Transform>();
    return ent;
}

void Scene::deleteEntity(Terra::Entity& ent){
    if (ent.hasComponent<Component::SpriteRenderer>())
        Renderer::deleteFromBatch(ent.getComponent<Component::SpriteRenderer>());
    registry.destroy(ent.getInnerEntity());
}

