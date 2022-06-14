#include "scripts/characterController.h"
#include "renderer/renderer.h"
#include "core/input.h"


void CharacterController::start(){
    entity.addComponent<Component::physicsBody>();
    entity.addComponent<Component::AABB>(1.0f, 1.3f);

    aniController = &entity.addComponent<Component::AnimationController>();
    Ref<Texture> tex = AssetPool::getTexture("/Users/shauntemellor/Documents/Documents - Shaunte’s MacBook Pro/comsci/Projects/Terraria/assets/img/aubreySheet.png");
    sprRend = &entity.addComponent<Component::SpriteRenderer>(SpriteSheet{tex, 4, 3}, glm::vec4(1.0f, 1.0f, 1.0f, 0.5f));

    body = &entity.getComponent<Component::physicsBody>();
    trans = &entity.getComponent<Component::Transform>();
    trans->setPos(glm::vec2(0.0f, 30.0f));
    trans->scale = glm::vec2(4, 4);

    Animation ani{1, 0, 3, 0.1f};
    aniController->addAnimation(ANIMATION_TYPE::WALK_LEFT, ani);
    Animation ani2{2, 0, 3, 0.1f};
    aniController->addAnimation(ANIMATION_TYPE::WALK_RIGHT, ani2);
    aniController->setCurrentAnimation(ANIMATION_TYPE::WALK_LEFT);
    scene->addToRenderer(entity);
}

static bool pressed = false;
static glm::vec2 prevVelocity;
static bool freeCam = false;

void CharacterController::update(float timeStep){
    float speed = 10.0f;
    float camSpeed = 15.0f;
    float minCamDist = 1e-5;

    if (KeyListener::isKeyPressed(GLFW_KEY_1))
        freeCam = !freeCam;
    
    if (!freeCam){
            prevVelocity = body->velocity;
            if (KeyListener::isKeyPressed(GLFW_KEY_S) && !pressed){
                body->velocity += glm::vec2(0.0f, -speed);
                pressed = true;
            }
            else if (KeyListener::isKeyPressed(GLFW_KEY_W) && !pressed){
                body->velocity += glm::vec2(0.0f, speed);
                pressed = true;
            }
            else if (KeyListener::isKeyPressed(GLFW_KEY_A) && !pressed){
                body->velocity += glm::vec2(-speed, 0.0f);
                pressed = true;
            }
            else if (KeyListener::isKeyPressed(GLFW_KEY_D) && !pressed){
                body->velocity += glm::vec2(speed, 0.0f);
                pressed = true;
            }
            else{
                body->velocity = prevVelocity;
                pressed = false;
            }
    }
    else{
            float speed = 80.0f * timeStep;
            if (KeyListener::isKeyPressed(GLFW_KEY_S))
                Camera::changePosition(Camera::getPosition() + glm::vec2(0.0f, 1.0f) * speed);
            else if (KeyListener::isKeyPressed(GLFW_KEY_W))
                Camera::changePosition(Camera::getPosition() - glm::vec2(0.0f, 1.0f)* speed);
            else if (KeyListener::isKeyPressed(GLFW_KEY_A))
                Camera::changePosition(Camera::getPosition() + glm::vec2(1.0f, 0.0f) * speed);
            else if (KeyListener::isKeyPressed(GLFW_KEY_D))
                Camera::changePosition(Camera::getPosition() - glm::vec2(1.0f, 0.0f) * speed);
    }
     
    //gravity
    body->accerlation = glm::vec2(0.0f, -10.0f);

    //jump
    if (KeyListener::isKeyPressed(GLFW_KEY_SPACE))
        body->addForce(glm::vec2(0.0f, 100.0f));

    if (!freeCam){
            glm::vec2 camDisp = trans->position - Camera::getPosition();
            if ( (glm::length(camDisp) >= minCamDist))
                Camera::changePosition(Camera::getPosition() + camSpeed * timeStep * camDisp);
    }
    //Camera::changePosition(trans->position);

    if (trans->position.x - lastPos.x < 0)
        aniController->setCurrentAnimation(ANIMATION_TYPE::WALK_RIGHT);
    else if (trans->position.x - lastPos.x > 0)
        aniController->setCurrentAnimation(ANIMATION_TYPE::WALK_LEFT);

    aniController->advance(trans->position.x - lastPos.x == 0 ? 0.0f : timeStep);
    const Animation& ani = aniController->getCurrentAnimation();
    sprRend->changeSprite(Sprite{ani.row, ani.currentFrame});

    lastPos = trans->position;
}

void CharacterController::onDestroy(){
    std::cout << "I'm Dead!\n";
}

void CharacterController::onCollision(const CollisionData& collision){
    if (collision.pos.y < trans->position.y && floor(trans->position.x) == floor(collision.pos.x))// && canCollide){
        body->velocity.y = 0.0f;


    Component::AABB& c = entity.getComponent<Component::AABB>();
    float diff = 0.10f;
    int bottomY = (int) floor(trans->position.y - c.extentsY) + 1;
    int x = (int) floor(trans->position.x);
  
    TileType tile = scene->getWorld()->tileTypeAtWorldCoordinate(x + 2, bottomY);
    TileType above = scene->getWorld()->tileTypeAtWorldCoordinate(x + 2, bottomY + 1);
    if (tile != TileType::air && above == TileType::air){
        trans->setPos(trans->position + glm::vec2(0.0f, 1.0f) + diff);
    }
}