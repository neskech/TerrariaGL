#include "scripts/Animation.h"
#include "renderer/renderer.h"
#include "core/input.h"

class Camera;

void AnimationScript::start(){
    aniController = &entity.addComponent<Component::AnimationController>();
    Ref<Texture> tex = AssetPool::getTexture("/Users/shauntemellor/Documents/Documents - Shaunte’s MacBook Pro/comsci/Projects/Terraria/assets/img/ani.png");
    sprRend = &entity.addComponent<Component::SpriteRenderer>(SpriteSheet{tex, 4, 12});

    Animation ani{1, 0, 12, 0.1f};
    aniController->addAnimation(ANIMATION_TYPE::WALK_LEFT, ani);
    aniController->setCurrentAnimation(ANIMATION_TYPE::WALK_LEFT);
    Renderer::submit(entity);
}

void AnimationScript::update(float timeStep){
    float speed = 80.0f * timeStep;
    if (KeyListener::isKeyPressed(GLFW_KEY_S))
        Camera::changePosition(Camera::getPosition() + glm::vec2(0.0f, 1.0f) * speed);
    else if (KeyListener::isKeyPressed(GLFW_KEY_W))
        Camera::changePosition(Camera::getPosition() - glm::vec2(0.0f, 1.0f)* speed);
    else if (KeyListener::isKeyPressed(GLFW_KEY_A))
         Camera::changePosition(Camera::getPosition() + glm::vec2(1.0f, 0.0f) * speed);
    else if (KeyListener::isKeyPressed(GLFW_KEY_D))
         Camera::changePosition(Camera::getPosition() - glm::vec2(1.0f, 0.0f) * speed);

    aniController->advance(timeStep);
    const Animation& ani = aniController->getCurrentAnimation();
    sprRend->changeSprite(Sprite{ani.row, ani.currentFrame});
}

void AnimationScript::destroy(){
    
}