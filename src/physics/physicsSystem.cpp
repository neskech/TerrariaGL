#include "physics/physicsSystem.h"
#include "pch.h"
#include "renderer/debugRenderer.h"


static std::optional<CollisionData> isTileCollison(int startX, int width, int startY, int height, World* world){

    int y_cord = abs(startY - CHUNK_HEIGHT / 2);

    for (int x = 0; x < width; x++){
        int chunkIndex = (startX + x) / CHUNK_WIDTH;
        if (startX + x < 0 && (startX + x) % CHUNK_WIDTH == 0)
             chunkIndex += 1;

        int xx = startX + x - chunkIndex * CHUNK_WIDTH; 
        xx += xx < 0 ? CHUNK_WIDTH : 0;

        Node<Chunk>* chunk = world->getChunkAtXCoordinate(startX + x);
        auto& chunkArray = chunk->value->tiles;
        for (int y = 0; y < height; y++){
             if (y + y_cord >= CHUNK_HEIGHT)
                break;

             TileType tile = chunkArray.at((y + y_cord) * CHUNK_WIDTH + xx);
             if (tile != TileType::air && tile < TileType::background)
                 return CollisionData(glm::vec2(startX + x + 0.5f, startY - y - 0.5f), Component::AABB(0.5f, 0.5f));    

        }
  
    }

    return {};
}

static std::optional<CollisionData> isAABBCollision(const Component::AABB& c1, const Component::AABB& c2, const glm::vec2& pos1, const glm::vec2& pos2){
        //Minimum of the two right edges >= Maximum of the two left edges
        bool xCollision = pos1.x + c1.extentsX >= pos2.x && pos1.x <= pos2.x + c2.extentsX;
        bool yCollision = pos1.y + c1.extentsY >= pos2.y && pos1.y <= pos2.y + c2.extentsY;
        
        if (xCollision && yCollision)
            CollisionData(pos2, c2);
        return {};
}

static std::optional<CollisionData> resolveAABBCollision(const Component::AABB& c1, const Component::AABB& c2, Component::Transform& trans, const glm::vec2& pos2, Component::physicsBody& body, float timeStep){

    float oldXComponent = trans.position.x;
    trans.position.x += body.velocity.x * timeStep + 0.5f * body.accerlation.x * timeStep * timeStep;
    
    std::optional<CollisionData> collidedX = isAABBCollision(c1, c2, trans.position, pos2);
    if (collidedX)
        trans.position.x = oldXComponent;

    float oldYComponent = trans.position.y;
    trans.position.y += body.velocity.y * timeStep + 0.5f * body.accerlation.y * timeStep * timeStep;

    std::optional<CollisionData> collidedY = isAABBCollision(c1, c2, trans.position, pos2);
    if (collidedY)
        trans.position.y = oldYComponent;

    trans.dirty = !(oldXComponent == trans.position.x && oldYComponent == trans.position.y);

    body.velocity += body.accerlation * timeStep;

    if (collidedX && !collidedY)
        return collidedX;
    return collidedY;

}

static std::optional<CollisionData> resolveTileCollision(const glm::ivec2& dimensions, const Component::AABB& collider, Component::Transform& trans, Component::physicsBody& body, float timeStep, World* world){

    float oldXComponent = trans.position.x;
    trans.position.x += body.velocity.x * timeStep + 0.5f * body.accerlation.x * timeStep * timeStep;

    int startX = (int) floor(trans.position.x - collider.extentsX);
    int startY = (int) floor(trans.position.y + collider.extentsY);
    
    std::optional<CollisionData> collidedX = isTileCollison(startX, dimensions.x, startY, dimensions.y, world);
    if (collidedX)
        trans.position.x = oldXComponent;

    float oldYComponent = trans.position.y;
    trans.position.y += body.velocity.y * timeStep + 0.5f * body.accerlation.y * timeStep * timeStep;

    startX = (int) floor(trans.position.x - collider.extentsX);
    startY = (int) floor(trans.position.y + collider.extentsY);

    std::optional<CollisionData> collidedY = isTileCollison(startX, dimensions.x, startY, dimensions.y, world);
    if (collidedY)
        trans.position.y = oldYComponent;

    trans.dirty = !(oldXComponent == trans.position.x && oldYComponent == trans.position.y);

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

    //then iterate through those with scripts
    auto view2 = reg.view<Component::physicsBody, Component::AABB, Component::Transform, Component::Script>();
    for (auto entity : view2){
            auto& body = view2.get<Component::physicsBody>(entity);
            auto& trans = view2.get<Component::Transform>(entity);
            auto& AABB = view2.get<Component::AABB>(entity);
            auto& script = view2.get<Component::Script>(entity);

            glm::ivec2 dimensions((int) floor(AABB.extentsX * 2.0f + 1), (int) floor(AABB.extentsY * 2.0f + 1));
            for (int i = 0; i < NUM_PHYSICS_STEPS; i++){
                float STEP = (timeStep / NUM_PHYSICS_STEPS) * (i+1);
                std::optional<CollisionData> data = resolveTileCollision(dimensions, AABB, trans, body, STEP, world);
                if (data)
                    script.onCollision(data.value());

                body.velocity += body.accerlation * timeStep;
            
            }

    }  
}
