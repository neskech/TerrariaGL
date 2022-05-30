#pragma once


#include "scene/scene.h"
#include "include/entt/entt.hpp"

namespace Terra{

class Entity{
    public:

        Entity(entt::entity ent_, Scene* scene_): ent(ent_), scene(scene_){}

        template <class T>
        T& getComponent() {
            if (!hasComponent<T>()){
                std::cerr << "ERROR: In Entity::getComponent. Check hasComponent() before getting a component!\n";
                assert(false);
            }

            return scene->getRegistry().get<T>(ent);
        }

        template <class T>
        T& getorAddComponent(){
            return scene->getRegistry().get_or_emplace<T>(ent);
        }

        template <class T>
        bool hasComponent(){
            return scene->getRegistry().any_of<T>(ent);
        }

        template <class T, typename... Args>
        T& addComponent(Args&&... args){
            return scene->getRegistry().emplace<T>(ent, std::forward<Args>(args)...);
        }

        template <class T>
        void removeComponent(){
             if (!hasComponent<T>()){
                std::cerr << "ERROR: In Entity::getComponent. Check hasComponent() before getting a component!\n";
                assert(false);
             }

             scene->getRegistry().remove<T>(ent);
        }

        inline entt::entity getInnerEntity(){ return ent; }
    private:
        friend class Scene;
        entt::entity ent;
        Scene* scene;
};

}

