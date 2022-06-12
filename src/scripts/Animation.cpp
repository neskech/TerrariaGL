#include "scripts/Animation.h"
#include "renderer/renderer.h"
#include "core/input.h"

class Camera;

void AnimationScript::start(){
    aniController = &entity.addComponent<Component::AnimationController>();
    Ref<Texture> tex = AssetPool::getTexture("/Users/shauntemellor/Documents/Documents - Shaunte’s MacBook Pro/comsci/Projects/Terraria/assets/img/aubreySheet.png");
    sprRend = &entity.addComponent<Component::SpriteRenderer>(SpriteSheet{tex, 4, 3});

    body = &entity.getComponent<Component::physicsBody>();

    background = &scene->getEntityByTagName("Background")->getComponent<Component::Transform>();
    background->setPos(glm::vec2(0.0f, 15.0f));
    trans = &entity.getComponent<Component::Transform>();
    trans->setPos(glm::vec2(0.0f, 30.0f));

    Animation ani{1, 0, 3, 0.1f};
    aniController->addAnimation(ANIMATION_TYPE::WALK_LEFT, ani);
    Animation ani2{2, 0, 3, 0.1f};
    aniController->addAnimation(ANIMATION_TYPE::WALK_RIGHT, ani2);
    aniController->setCurrentAnimation(ANIMATION_TYPE::WALK_LEFT);
    scene->addToRenderer(entity);
}


void AnimationScript::update(float timeStep){
    float speed = 10.0f;
    float camSpeed = 15.0f;
    float minCamDist = 1e-5;
    if (KeyListener::isKeyPressed(GLFW_KEY_S))
        body->velocity = glm::vec2(0.0f, -speed);
    if (KeyListener::isKeyPressed(GLFW_KEY_W))
        body->velocity = glm::vec2(0.0f, speed);
    if (KeyListener::isKeyPressed(GLFW_KEY_A))
        body->velocity = glm::vec2(-speed, 0.0f);
    if (KeyListener::isKeyPressed(GLFW_KEY_D))
        body->velocity = glm::vec2(speed, 0.0f);
   // else
     //   body->velocity *= 0.0f;
     
     body->accerlation = glm::vec2(0.0f, -10.0f);
     if (KeyListener::isKeyPressed(GLFW_KEY_SPACE))
        body->addForce(glm::vec2(0.0f, 100.0f));

    int chunkCord = (int) floor(trans->position.x / CHUNK_WIDTH);
  //  chunkCord = chunkCord < 0 ? chunkCord - 1 : chunkCord;
    std::cout << " chunk cord " << chunkCord << std::endl;

    glm::vec2 camDisp = trans->position - Camera::getPosition();
    if ( (glm::length(camDisp) >= minCamDist))
        Camera::changePosition(Camera::getPosition() + camSpeed * timeStep * camDisp);
        
   // Camera::changePosition(trans->position);
    background->setPos(glm::vec2(Camera::getPosition().x, background->position.y));

    if (trans->position.x - lastPos.x < 0)
        aniController->setCurrentAnimation(ANIMATION_TYPE::WALK_RIGHT);
    else if (trans->position.x - lastPos.x > 0)
        aniController->setCurrentAnimation(ANIMATION_TYPE::WALK_LEFT);

    aniController->advance(trans->position.x - lastPos.x == 0 ? 0.0f : timeStep);
    const Animation& ani = aniController->getCurrentAnimation();
    sprRend->changeSprite(Sprite{ani.row, ani.currentFrame});

    lastPos = trans->position;

    //DebugRenderer::drawLine(glm::vec2(0.0f, 12.0f), glm::vec2(10.0f, 12.0f), glm::vec3(1.0f, 1.0f, 1.0f));
   // DebugRenderer::drawLine(glm::vec2(0.0f, 0.0f), 100.0f * glm::vec2((float)cos(theta), (float)sin(theta)), glm::vec3(0.4f, 0.9f, 0.8f));
    Component::AABB& c = entity.getComponent<Component::AABB>();
    //DebugRenderer::drawRect(trans->position - glm::vec2(c.extentsX, c.extentsY), trans->position + glm::vec2(c.extentsX, c.extentsY),
    //glm::vec3(1.0f, 1.0f, 1.0f));
}

void AnimationScript::onDestroy(){
    
}
bool canCollide = true;
void AnimationScript::onCollision(const Component::CollisionData& collision){
    if (collision.pos.y < trans->position.y && floor(trans->position.x) == floor(collision.pos.x))// && canCollide){
        body->velocity.y = 0.0f;
        //canCollide= false;
    //}
   // else if (collision.pos.y > trans->position.y) canCollide = true;


    Component::AABB& c = entity.getComponent<Component::AABB>();
    float diff = 0.10f;
    int bottomY = (int) floor(trans->position.y - c.extentsY) + 1;
    int x = (int) floor(trans->position.x);
  
    BlockType tile = scene->getWorld()->blockTypeAtWorldCoordinate(x + 2, bottomY);
    BlockType above = scene->getWorld()->blockTypeAtWorldCoordinate(x + 2, bottomY + 1);
    if (tile != BlockType::air && above == BlockType::air){
       //body->velocity += glm::vec2(0.0f, 3.0f + diff);
       // body->addForce(glm::vec2(0.0f, 50.0f));
        trans->setPos(trans->position + glm::vec2(0.0f, 1.0f) + diff);
    }

    // tile = scene->getWorld()->blockTypeAtWorldCoordinate(x - 2, bottomY);
    // if (tile != BlockType::air) 
    //     trans->setPos(trans->position + glm::vec2(0.0f, 1.0f));

   // DebugRenderer::drawRect(glm::vec2(collision.pos - glm::vec2(collision.collider.extentsX, collision.collider.extentsY)),
   // glm::vec2(collision.pos + glm::vec2(collision.collider.extentsX, collision.collider.extentsY)));

    // DebugRenderer::drawRect(glm::vec2(x, bottomY - 1), glm::vec2(x + 1, bottomY));
}