#pragma once
#include <stdlib.h> 
#include <vector>
#include "pch.h"

#include "scene/entity.h"

//TODO Have a MAX_ENTITIES constant to ensure nobody goes over it, assert(false) otherwise

//Cannot template this, because we need to store an array of them
struct ComponentPool{
    char* data = nullptr;
    std::size_t length;
    std::size_t typeSize;

    ComponentPool(std::size_t baseLength_, std::size_t typeSize_){
        data = new char[baseLength_ * typeSize_];
        typeSize = typeSize_;
        length = baseLength_;
    }

    ~ComponentPool(){
        delete[] data;
    }

    inline void* get(std::size_t index){
        return data + index * typeSize;
    }

    void resize(std::size_t padding){
        data = (char*)realloc(data, length + padding);
        length += padding;
    }

};

constexpr std::size_t COMPONENT_POOL_PAD = 10;
constexpr std::size_t COMPONENT_POOL_BASE_LENGTH = 10;

class Registry{
    public:
        Registry(){
            if (instance != nullptr)
                assert(false);
            instance = this;
        }

        Registry(Registry& other) = delete;
        Registry(const Registry& other) = delete;
        Registry(Registry&& other) = delete;

        ~Registry(){}

        template <class T>
        uint16_t getID(){
            //assignment of static variable is invalid after the first call, so it's stuck with its first value
            //Different values of compID are present for each templated version of the function the compiler makes
            static int compID = componentCount++;
            return compID;
        }

        template <class T, typename... Args>
        T& addComponent(Entity* e, Args&&... args){
            uint16_t compID = getID<T>();

            if (!hasComponent<T>(e->getID())){
                std::cerr << "ERROR: In registry::addComponent. Entity already has specified component!\n";
                assert(false);
            }

            //to prevent uncessary padding
            if (compID >= pools.size())
                pools.resize(compID + 1);
            
            if (POOLS[compID] == nullptr)
                POOLS[compID] = std::make_unique<ComponentPool>(COMPONENT_POOL_BASE_LENGTH, sizeof(T));

            if (e->getID() >= pools[compID]->length)
                pools[compID]->resize(COMPONENT_POOL_PAD);

            e->getBitset().set(compID);

            //In place construct a new T element into the component pool
            T* component = new (pools[compID]->get(e->getID())) T(std::forward<Args>(args)...);
            return *component;
            
        }

        template <class T>
        bool hasComponent(Entity* e){
            uint16_t compID = getID<T>();
            return compID < pools.size() && e->getID() < pools[compID]->length && pools[compID]->get(e->getID()) != nullptr;
        }

        template <class T>
        T& getComponent(Entity* e){
            uint16_t compID = getID<T>();
            
            if (!hasComponent<T>(e->getID())){
                std::cerr << "ERROR: In registry::getComponent. Entity does not have required component. Check hasComponent() before running this function!\n";
                assert(false);
            }

            return *( static_cast<T*>(pools[compID]->get(e->getID())) );
        }

        template <class T>
        void removeComponent(Entity* e){
            uint16_t compID = getID<T>();

            if (!hasComponent<T>(e->getID())){
                std::cerr << "ERROR: In registry::removeComponent. Entity does not have required component. Check hasComponent() before running this function!\n";
                assert(false);
            }

            delete ( static_cast<T*>(pools[compID]->get(e->getID())) );
            e->getBitset().reset(compID);
        }

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool hasComponent(Entity* e, uint16_t compID){
            return compID < pools.size() && e->getID() < pools[compID]->length && pools[compID]->get(e->getID()) != nullptr;
        }

        void removeComponent(Entity* e, uint16_t compID){

            if (!hasComponent(e, compID)){
                std::cerr << "ERROR: In registry::removeComponent. Entity does not have required component. Check hasComponent() before running this function!\n";
                assert(false);
            }

            delete ( static_cast<T*>(pools[compID]->get(e->getID())) );
            e->getBitset().reset(compID);
        }

    private:
        uint16_t componentCount;
        std::vector<Scoped<ComponentPool>> pools;

        static Registry* instance;
};

Registry* Registry::instance = nullptr;