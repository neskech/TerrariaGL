#pragma once
#include "pch.h"
#include "scene/camera.h"
#include "world/world.h"

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
        void removeFromRenderer(Terra::Entity& ent);
        Terra::Entity& createEntity(const std::string& = "None");
        void deleteEntity(Terra::Entity& ent, bool removeFromRenderer = true);

        void deleteEntityByTagName(const std::string& tagName);
        Terra::Entity* getEntityByTagName(const std::string& tagName);

        inline entt::registry& getRegistry(){ return registry; }
        inline World* getWorld(){ return world; }
    private:
        entt::registry registry;
        std::vector<Terra::Entity> entites;

        World* world;
        Renderer* renderer;
        Camera* camera;
};