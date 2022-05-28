#pragma once
#include <bits/stdc++.h>
#include "scene/scene.h"
#include "pch.h"

#define MAX_COMPONENTS 32

class Entity{
    public:
        Entity(Scene* scene_): scene(scene_){
            ID = ID_COUNT++;
        }
        
        ~Entity(){
            scene->deleteEntity(*this);
        }

        template <class T, typename... Args>
        T& getComponent(Args&&... args){
            if (!hasComponent<T>()){
                std::cerr << "ERROR: In Entity::getComponent. Check hasComponent() before getting a component!\n";
                assert(false);
            }

            return scene->getRegistry()->getComponent<T>(this);
        }

        template <class T>
        bool hasComponent(){
            return bits.test(scene->getRegistry()->getID<T>());
        }

        template <class T>
        T& addComponent(){
            return scene->getRegistry()->addComponent<T>(this);
        }

        template <class T>
        void removeComponent(){
             if (!hasComponent<T>()){
                std::cerr << "ERROR: In Entity::getComponent. Check hasComponent() before getting a component!\n";
                assert(false);
             }

             scene->getRegistry()->removeComponent<T>(this);
        }

        inline unsigned long long getID(){ return ID; }
        inline std::bitset<MAX_COMPONENTS>& getBitset(){ return bits; }


    private:
        unsigned long long ID;
        static long long ID_COUNT;
        std::bitset<MAX_COMPONENTS> bits;
        Scene* scene;
};