#include "renderer/renderer.h"

Renderer* Renderer::instance = nullptr;

Renderer::Renderer(){
    if (instance != nullptr)
        assert(false);

    instance = this;
    instance->batches.reserve(1);
}

Renderer::~Renderer(){
    for (RenderBatch* batch: instance->batches)
        delete batch;
}

void Renderer::render(){
    for (auto batch : instance->batches){
        batch->render();
    }
}

//make sure the batch isnt full (batch size and textures). If full, make a new one 
//Otherwise add the spr to the batch
void Renderer::addToBatch(Component::SpriteRenderer& spr){
    for (auto batch : instance->batches){
        if (batch->getCurrentSize() < MAX_BATCH_SIZE && (batch->getnumTextures() < MAX_TEXTURES || batch->containsTexture(spr.sheet.tex))){
            batch->addSpriteRenderer(&spr);
            return;
        }
    }

    //resize manually to avoid too much padding
    if (instance->batches.size() == instance->batches.capacity())
         instance->batches.resize(instance->batches.size() + 1);

    //placement new so that elements are placed in continous memory
    //RenderBatch* b = new (&instance->batches[0] + instance->batches.size()) RenderBatch();
    instance->batches.emplace_back();
    instance->batches.back()->addSpriteRenderer(&spr);
}

void Renderer::deleteFromBatch(Component::SpriteRenderer& spr){
    using map = std::unordered_map<Component::SpriteRenderer*, uint16_t>;

    for (auto batch: instance->batches){
        map& m = batch->getRenderers();
        if (m.find(&spr) != m.end()){
            batch->removeSpriteRenderer(&spr);
            return;
        }
    }
}