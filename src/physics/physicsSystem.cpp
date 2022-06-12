#include "physics/physicsSystem.h"
#include "pch.h"
#include "renderer/debugRenderer.h"
#include <optional>


static std::optional<Component::CollisionData> isTileCollison(int startX, int width, int startY, int height, World* world){

    int y_cord = abs(startY - CHUNK_HEIGHT / 2);

    Node<Chunk>* currentChunk = world->getCurrentChunkNode();
    int chunkIndexInArray = world->getChunkIndexInArray(world->getCurrentChunkIndex());
    for (int x = 0; x < width; x++){
        int chunkIndex = (startX + x) / CHUNK_WIDTH;
        if (startX + x < 0 && (startX + x) % CHUNK_WIDTH == 0)
             chunkIndex += 1;

        int chunkIndexA = world->getChunkIndexInArray(startX + x < 0 ? chunkIndex - 1 : chunkIndex);

        int xx = startX + x - chunkIndex * CHUNK_WIDTH; 
        xx += xx < 0 ? CHUNK_WIDTH : 0;

        auto& chunkArray = world->getChunksList().getNodeStartingFrom(currentChunk, chunkIndexInArray, chunkIndexA)->value->tiles;
        for (int y = 0; y < height; y++){
             if (y + y_cord >= CHUNK_HEIGHT)
                break;

             BlockType tile = chunkArray.at((y + y_cord) * CHUNK_WIDTH + xx);
             if (tile != BlockType::air)
                 return Component::CollisionData(glm::vec2(startX + x + 0.5f, startY - y - 0.5f), Component::AABB(0.5f, 0.5f));    

        }
  
    }

    return {};
}

static bool isAABBCollision(const Component::AABB& c1, const Component::AABB& c2, const glm::vec2& pos1, const glm::vec2& pos2){
        //Minimum of the two right edges >= Maximum of the two left edges
        bool xCollision = pos1.x + c1.extentsX >= pos2.x && pos1.x <= pos2.x + c2.extentsX;
        bool yCollision = pos1.y + c1.extentsY >= pos2.y && pos1.y <= pos2.y + c2.extentsY;

        return xCollision && yCollision;
}

static void resolveAABBCollision(const Component::AABB& c1, const Component::AABB& c2, Component::Transform& trans, const glm::vec2& pos2, Component::physicsBody& body, float timeStep){

    float oldXComponent = trans.position.x;
    trans.position.x += body.velocity.x * timeStep + 0.5f * body.accerlation.x * timeStep * timeStep;
    
    bool collidedX = isAABBCollision(c1, c2, trans.position, pos2);
    if (collidedX)
        trans.position.x = oldXComponent;

    float oldYComponent = trans.position.y;
    trans.position.y += body.velocity.y * timeStep + 0.5f * body.accerlation.y * timeStep * timeStep;

    bool collidedY = isAABBCollision(c1, c2, trans.position, pos2);
    if (collidedY)
        trans.position.y = oldYComponent;

    trans.dirty = true;

}

static std::optional<Component::CollisionData> resolveTileCollision(const glm::ivec2& dimensions, const Component::AABB& collider, Component::Transform& trans, Component::physicsBody& body, float timeStep, World* world){

    float oldXComponent = trans.position.x;
    trans.position.x += body.velocity.x * timeStep + 0.5f * body.accerlation.x * timeStep * timeStep;

    int startX = (int) floor(trans.position.x - collider.extentsX);
    int startY = (int) floor(trans.position.y + collider.extentsY);
    
    std::optional<Component::CollisionData> collidedX = isTileCollison(startX, dimensions.x, startY, dimensions.y, world);
    if (collidedX)
        trans.position.x = oldXComponent;

    float oldYComponent = trans.position.y;
    trans.position.y += body.velocity.y * timeStep + 0.5f * body.accerlation.y * timeStep * timeStep;

    startX = (int) floor(trans.position.x - collider.extentsX);
    startY = (int) floor(trans.position.y + collider.extentsY);

    std::optional<Component::CollisionData> collidedY = isTileCollison(startX, dimensions.x, startY, dimensions.y, world);
    if (collidedY)
        trans.position.y = oldYComponent;

    trans.dirty = true;
    // if ( (body.accerlation.y < 0 && (collidedX || collidedY))){
    //     body.accerlation.y = 0.0f;
    // }

    body.velocity += body.accerlation * timeStep;

    if (collidedX && !collidedY)
        return collidedX;
    return collidedY;
}

void simulate(entt::registry& reg, World* world, float timeStep){

    //iterate through non scripts first
    auto view = reg.view<Component::physicsBody, Component::AABB, Component::Transform>(entt::exclude<Component::Script>);
    for (auto entity : view){
            auto& body = view.get<Component::physicsBody>(entity);
            auto& trans = view.get<Component::Transform>(entity);
            auto& AABB = view.get<Component::AABB>(entity);

            glm::ivec2 dimensions((int) floor(AABB.extentsX * 2.0f + 1), (int) floor(AABB.extentsY * 2.0f + 1));
            for (int i = 0; i < NUM_PHYSICS_STEPS; i++){
                float STEP = (timeStep / NUM_PHYSICS_STEPS) * (i+1);
                resolveTileCollision(dimensions, AABB, trans, body, STEP, world);
            
            }

    }  

    //then iterate through scripts
    auto view2 = reg.view<Component::physicsBody, Component::AABB, Component::Transform, Component::Script>();
    for (auto entity : view2){
            auto& body = view2.get<Component::physicsBody>(entity);
            auto& trans = view2.get<Component::Transform>(entity);
            auto& AABB = view2.get<Component::AABB>(entity);
            auto& script = view2.get<Component::Script>(entity);

            glm::ivec2 dimensions((int) floor(AABB.extentsX * 2.0f + 1), (int) floor(AABB.extentsY * 2.0f + 1));
            for (int i = 0; i < NUM_PHYSICS_STEPS; i++){
                float STEP = (timeStep / NUM_PHYSICS_STEPS) * (i+1);
                std::optional<Component::CollisionData> data = resolveTileCollision(dimensions, AABB, trans, body, STEP, world);
                if (data)
                    script.onCollision(data.value());

                body.velocity += body.accerlation * timeStep;
            
            }

    }  
}
