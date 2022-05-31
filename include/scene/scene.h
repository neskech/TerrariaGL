#pragma once
#include "pch.h"
#include "scene/camera.h"
#include <vector>

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

        void addToRenderer(Terra::Entity& ent);
        Terra::Entity& createEntity(const char* name = nullptr);
        void deleteEntity(Terra::Entity& ent);
        void deleteEntityByTagName(const std::string& tagName);

        inline entt::registry& getRegistry(){ return registry; }
    private:
        entt::registry registry;
        std::vector<Terra::Entity> entites;
        Renderer* renderer;
        Camera* camera;

};