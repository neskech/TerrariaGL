#pragma once
#include "scene/entity.h"

class Component{
    public:
        Component();
        virtual ~Component();

        inline void setEntity(Entity& ent_){ ent = ent_; }
        inline const Entity& getEntity(){ return ent; }
    protected:
        Entity& ent;
};