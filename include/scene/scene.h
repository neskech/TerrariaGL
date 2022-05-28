#pragma once
#include "scene/registry.h"
#include <vector>

//TODO implement checking so that entity ID does not go outside the bound of MAX_ENTITIES
class Scene{
    public:
        Scene();
        ~Scene();

        void init();
        void update();
        void render();

        Entity createEntity();
        void deleteEntity(Entity e);

        inline Registry* getRegistry(){ return registry; }
    private:
        Registry* registry;
        std::vector<Entity> entities;
};