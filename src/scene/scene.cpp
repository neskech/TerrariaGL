#include "scene/scene.h"

Scene::Scene(){
    registry = new Registry();
}

Scene::~Scene(){
    delete registry;
}

void Scene::init(){

}

void Scene::update(){

}

void Scene::render(){

}

Entity Scene::createEntity(){
    Entity ent(this);
    return ent;
}

void Scene::deleteEntity(Entity e){
    for (int i = 0; i < e.getBitset().size(); i++){
        if (e.getBitset().test(i))
        
    }
}
