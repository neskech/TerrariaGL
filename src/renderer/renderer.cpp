#include "renderer/renderer.h"

Renderer* Renderer::instance = nullptr;

Renderer::Renderer(){
    if (instance != nullptr)
        assert(false);

    instance = this;
}

Renderer::~Renderer(){
    //TODO Find out if this accounts for the extra batches allocated by realloc
    delete[] instance->instanceBatches;
    // or just do
    // for (int i = 0; i < instance->numBatches; i++) free(instance->instanceBatches + i);
}

void Renderer::init(){
    instance->instanceBatches = new InstanceRenderer[1];
    instance->instanceBatches[0].init();
    instance->numBatches = 1;
}

void Renderer::render(){
    for (int i = 0; i < instance->numBatches; i++){
        instance->instanceBatches[i].render();
    }
}

//make sure the batch isnt full (batch size and textures). If full, make a new one 
//Otherwise add the spr to the batch
void Renderer::addToBatch(Component::SpriteRenderer& spr){
    for (int i = 0; i < instance->numBatches; i++){
        if (instance->instanceBatches[i].getCurrentSize() < MAX_INSTANCES && (instance->instanceBatches[i].getnumTextures() < MAX_TEXTURES || instance->instanceBatches[i].containsTexture(spr.sheet.tex))){
            instance->instanceBatches[i].addSpriteRenderer(&spr);
            return;
        }
    }
    //resize manually to avoid too much padding

    instance->instanceBatches = (InstanceRenderer*) realloc(instance->instanceBatches, sizeof(InstanceRenderer) * (instance->numBatches + 1));
    //placement new so that elements are placed in continous memory
    //InstanceRenderer* b = new (&instance->instanceBatches[0] + instance->instanceBatches.size()) InstanceRenderer();
    InstanceRenderer* b = new (instance->instanceBatches + instance->numBatches) InstanceRenderer();

    instance->instanceBatches[instance->numBatches].init();
    instance->instanceBatches[instance->numBatches].addSpriteRenderer(&spr);
    instance->numBatches++;

    //instance->instanceBatches[instance->instanceBatches.size() - 1]->init();

}

void Renderer::deleteFromBatch(Component::SpriteRenderer& spr){
    using map = std::unordered_map<Component::SpriteRenderer*, uint16_t>;

    for (int i = 0; i < instance->numBatches; i++){
        map& m = instance->instanceBatches[i].getRenderers();
        if (m.find(&spr) != m.end()){
            instance->instanceBatches[i].removeSpriteRenderer(&spr);
            return;
        }
    }
}