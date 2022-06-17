#include "scripts/characterController.h"
#include "renderer/renderer.h"
#include "core/input.h"

#define CAM_SPEED 15.0f
#define MIN_CAM_DIST 1e-5

#define FREE_CAM_SWAP_DELAY 0.5f
#define FREE_CAM_SPEED 80.0f

#define GRAV_DISABLE_STEPS 5
static bool disableGrav = false;
static int disableSteps = 0;

#define PLAYER_SPEED_NORMAL 20.0f
#define SUPER_SPEED_VELOCITY 40.0f
#define SUPER_SPEED_ENABLED_DELAY 1.0f

#define GRAVITY_STRENGTH 20.0f
#define GRAVITY_DISABLED_DELAY 1.0f

void CharacterController::start(){
    entity.addComponent<Component::physicsBody>();
    entity.addComponent<Component::AABB>(1.0f, 1.3f);

    aniController = &entity.addComponent<Component::AnimationController>();


    Ref<Texture> tex = AssetPool::getTexture("../assets/img/aubreySheet.png");
    sprRend = &entity.addComponent<Component::SpriteRenderer>(SpriteSheet{tex, 4, 3}, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    body = &entity.getComponent<Component::physicsBody>();
    trans = &entity.getComponent<Component::Transform>();
    trans->setPos(glm::vec2(0.0f, 30.0f));
    trans->scale = glm::vec2(4, 4);
    trans->setZIndex(2);

    Animation ani{1, 0, 3, 0.1f};
    aniController->addAnimation(ANIMATION_TYPE::WALK_LEFT, ani);
    Animation ani2{2, 0, 3, 0.1f};
    aniController->addAnimation(ANIMATION_TYPE::WALK_RIGHT, ani2);
    aniController->setCurrentAnimation(ANIMATION_TYPE::WALK_LEFT);
    scene->addToRenderer(entity);
}


void CharacterController::update(float timeStep){ 
    handleSettings(timeStep);

    if (freeCam)
        cameraMovementLogic(timeStep);
    else{

        playerMovementLogic();
        //lagging camera
        glm::vec2 camDisp = trans->position - Camera::getPosition();
        if ((glm::length(camDisp) >= MIN_CAM_DIST))
            Camera::changePosition(Camera::getPosition() + CAM_SPEED * timeStep * camDisp);
    }


    if (disableGrav){
        disableSteps++;
        if (disableSteps == GRAV_DISABLE_STEPS){
            disableSteps = 0;
            disableGrav = false;
        }

    }
    else if (gravityEnabled)
      body->accerlation = glm::vec2(0.0f, -GRAVITY_STRENGTH);
    

    if (trans->position.x - lastPos.x < 0)
        aniController->setCurrentAnimation(ANIMATION_TYPE::WALK_RIGHT);
    else if (trans->position.x - lastPos.x > 0)
        aniController->setCurrentAnimation(ANIMATION_TYPE::WALK_LEFT);

    aniController->advance(trans->position.x - lastPos.x == 0 ? 0.0f : timeStep);
    const Animation& ani = aniController->getCurrentAnimation();
    sprRend->changeSprite(Sprite{ani.row, ani.currentFrame});

    lastPos = trans->position;
}

void CharacterController::handleSettings(float timeStep){

    if (KeyListener::isKeyPressed(GLFW_KEY_Q) && KeyListener::isKeyModdedBy(GLFW_KEY_Q, GLFW_KEY_LEFT_CONTROL) && superSpeedEnabledElapsedTime >= SUPER_SPEED_ENABLED_DELAY){
        superSpeedEnabledElapsedTime = 0.0f;
        superSpeed = !superSpeed;
    }

    if (KeyListener::isKeyPressed(GLFW_KEY_X) && KeyListener::isKeyModdedBy(GLFW_KEY_X, GLFW_KEY_LEFT_CONTROL) && freeCamSwapElapsedTime >= FREE_CAM_SWAP_DELAY){
        freeCamSwapElapsedTime = 0.0f;
        freeCam = !freeCam;
    }  

    if (KeyListener::isKeyPressed(GLFW_KEY_G) && KeyListener::isKeyModdedBy(GLFW_KEY_G, GLFW_KEY_LEFT_CONTROL) && gravityChangeElapsedTime >= GRAVITY_DISABLED_DELAY){
        gravityChangeElapsedTime = 0.0f;
        gravityEnabled = !gravityEnabled;
    }


    freeCamSwapElapsedTime += timeStep;
    superSpeedEnabledElapsedTime += timeStep;
    gravityChangeElapsedTime += timeStep;
}

void CharacterController::playerMovementLogic(){
    const float SPEED = superSpeed ? SUPER_SPEED_VELOCITY : PLAYER_SPEED_NORMAL;
    if (KeyListener::isKeyPressed(GLFW_KEY_D))
        body->velocity.x = SPEED;
    else if (KeyListener::isKeyPressed(GLFW_KEY_A))
        body->velocity.x = -SPEED;
    else
        body->velocity.x = 0.0f;

    if (KeyListener::isKeyPressed(GLFW_KEY_W) && (!isJumping || !gravityEnabled)){
        body->velocity.y = SPEED;
        isJumping = true;
    }
    else if (KeyListener::isKeyPressed(GLFW_KEY_S))
        body->velocity.y = -SPEED;
    else if (!gravityEnabled) //only do this if gravity isn't enabled so you don't go shooting off
        body->velocity.y = 0.0f;

}

void CharacterController::cameraMovementLogic(float timeStep){
    if (KeyListener::isKeyPressed(GLFW_KEY_S))
        Camera::changePosition(Camera::getPosition() - glm::vec2(0.0f, 1.0f) * FREE_CAM_SPEED * timeStep);
    else if (KeyListener::isKeyPressed(GLFW_KEY_W))
        Camera::changePosition(Camera::getPosition() + glm::vec2(0.0f, 1.0f)* FREE_CAM_SPEED * timeStep);
    else if (KeyListener::isKeyPressed(GLFW_KEY_A))
        Camera::changePosition(Camera::getPosition() - glm::vec2(1.0f, 0.0f) * FREE_CAM_SPEED * timeStep);
    else if (KeyListener::isKeyPressed(GLFW_KEY_D))
        Camera::changePosition(Camera::getPosition() + glm::vec2(1.0f, 0.0f) * FREE_CAM_SPEED * timeStep);
}

void CharacterController::onDestroy(){

}

void CharacterController::onCollision(const CollisionData& collision){

    if (collision.pos.y < trans->position.y){
        body->velocity.y = 0.0f;
        isJumping = false;
    }


    Component::AABB& c = entity.getComponent<Component::AABB>();
    float diff = 0.2f;
    int bottomY = (int) floor(trans->position.y - c.extentsY) + 1;
    int x = (int) floor(trans->position.x);
    
  
    TileType tile = scene->getWorld()->tileTypeAtWorldCoordinate(x + 2, bottomY);
    TileType above = scene->getWorld()->tileTypeAtWorldCoordinate(x + 2, bottomY + 1);
    if (tile != TileType::air && tile < TileType::background && (above == TileType::air || above >= TileType::background) && body->velocity.x > 0 ){
        DebugRenderer::drawRect(glm::vec2(x+2, bottomY - 1), glm::vec2(x + 3, bottomY));
        trans->setPos(trans->position + glm::vec2(0.0f, 1.0f + diff));
        disableGrav = true;

    }

    tile = scene->getWorld()->tileTypeAtWorldCoordinate(x - 2, bottomY);
    above = scene->getWorld()->tileTypeAtWorldCoordinate(x - 2, bottomY + 1);
    if (tile != TileType::air && tile < TileType::background && (above == TileType::air || above >= TileType::background) && body->velocity.x < 0){
        trans->setPos(trans->position + glm::vec2(0.0f, 1.0f + diff));
        disableGrav = true;
    }
}