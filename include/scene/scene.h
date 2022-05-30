#pragma once
#include "pch.h"
#include "scene/camera.h"
//#include "renderer/renderer.h"
//#include "scene/entity.h"
class Renderer;
namespace Terra{
    class Entity;
}

//TODO implement checking so that Terra::Entity ID does not go outside the bound of MAX_ENTITIES
class Scene{
    public:
        Scene();
        ~Scene();

        void init();
        void update(float timeStep);
        void render();

        void addToRenderer(entt::entity ent);
        Terra::Entity createEntity();
        void deleteEntity(Terra::Entity& ent);

        inline entt::registry& getRegistry(){ return registry; }
    private:
        entt::registry registry;
        Renderer* renderer;
        Camera* camera;
};