#pragma once
#include "pch.h"
#include "scene/camera.h"

//forward declarations
class Renderer;
namespace Terra{
    class Entity;
}


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