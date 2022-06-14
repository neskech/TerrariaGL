#include "scripts/backgroundManager.h"

#define DAY_PERIOD 3
#define Y_POS 15.0f

static constexpr int WIDTH = CAM_WIDTH;
static int START_POS = 0.0f;

void BackgroundManager::start(){
    std::cout << "STARTY START\n";
    Terra::Entity* player = scene->getEntityByTagName("Player");
    assert(player);
    playerTransform = &(player->getComponent<Component::Transform>());
    lastPos = playerTransform->position;

    currentBackground = BackgroundType::Day;
    backgrounds.reserve(3);

    lastBiome = scene->getWorld()->getBiomeTypeAt(playerTransform->position.x);

    initDay();
    initNight();
    initUnderGround();

    presentBackground(currentBackground);
    setZIndex(currentBackground, -1.0f);
    std::cout << "STARTY START\n";
}

void BackgroundManager::update(float timeStep){
    if (transitioning){
        transitionAlphaValue -= (1.0f / delays[transitionBackground - 1]) * timeStep;
        if (transitionAlphaValue <= 0.0f){
            removeBackground(currentBackground);
            setAlphaValue(currentBackground, 1.0f);
            setZIndex(transitionBackground, -1.0f);
            currentBackground = transitionBackground;
            transitioning = false;
            transitionAlphaValue = 1.0f;
            elapsedTime = 0.0f;
        }
        else
            setAlphaValue(currentBackground, transitionAlphaValue);

        setPosition(transitionBackground, Camera::getPosition().x);
        elapsedTime += timeStep;
    }

    //if (lastTime + timeStep >= DAY_PERIOD && lastTime < DAY_PERIOD){
      Biome b = scene->getWorld()->getBiomeTypeAt(playerTransform->position.x);
      if (b != lastBiome){
        transitioning = true;
        transitionBackground = currentBackground == BackgroundType::Day ? BackgroundType::Night : BackgroundType::Day;
        lastTime = 0.0f;
        lastBiome = b;
        
        setZIndex(transitionBackground, -2.0f);
        presentBackground(transitionBackground);
    }

    lastTime += timeStep;

    setPosition(currentBackground, Camera::getPosition().x);
}

void BackgroundManager::initDay(){
    backgrounds[BackgroundType::Day].reserve(1);
    delays[BackgroundType::Day - 1] = 0.6f;

    Ref<Texture> textures[1] = {
        AssetPool::getTexture("/Users/shauntemellor/Documents/Documents - Shaunte’s MacBook Pro/comsci/Projects/Terraria/assets/img/Shotty.png")
    };

    float p[1] = {
        1.0f,
    };

    setBackground(BackgroundType::Day, textures, p, 1);
}

void BackgroundManager::initNight(){
    backgrounds[BackgroundType::Night].reserve(1);
    delays[BackgroundType::Night - 1] = 0.6f;

    Ref<Texture> textures[1] = {
        AssetPool::getTexture("/Users/shauntemellor/Documents/Documents - Shaunte’s MacBook Pro/comsci/Projects/Terraria/assets/img/ngiht.png")
    };

    float p[1] = {
        1.0f,
    };

    setBackground(BackgroundType::Night, textures, p, 1);
}

void BackgroundManager::initUnderGround(){

}

void BackgroundManager::onDestroy(){

}

void BackgroundManager::setBackground(BackgroundType type, Ref<Texture>* textures, float* p, int length){
    for (int i = 0; i < length; i++){
        backgrounds[type].emplace_back();

        for (int j = 0; j < 3; j++){
            auto& ent = scene->createEntity();

            float shift = j == 0 ? -1.0f : (j == 1 ? 0.0f : 1.0f);

            ent.addComponent<Component::SpriteRenderer>(SpriteSheet(textures[i], 1, 1));
            auto& trans = ent.getComponent<Component::Transform>();
            trans.setScale(glm::vec2(WIDTH, CAM_HEIGHT));
            trans.setPos(glm::vec2(0.0f + WIDTH * shift, Y_POS));

            backgrounds[type][i].entities[j] = &ent;
        }

        backgrounds[type][i].parralax = p[i];
    }
}

void BackgroundManager::setAlphaValue(BackgroundType type, float a){
    for (auto& back : backgrounds[type]){
        for (Terra::Entity* ent : back.entities){
                auto& spr =  ent->getComponent<Component::SpriteRenderer>();
                spr.color.a = a;
                spr.dirty = true;
        }
    }
}

void BackgroundManager::setZIndex(BackgroundType type, float z){
    for (auto& back : backgrounds[type]){
        for (Terra::Entity* ent : back.entities){
                auto& trans = ent->getComponent<Component::Transform>();
                trans.zIndex = z;
                trans.dirty = true;
        }
    }
}

void BackgroundManager::setPosition(BackgroundType type, float x){
    float shifts[3] = {1.0f, 0.0f, -1.0f};
    for (auto& back : backgrounds[type]){
        int i = 0;
        for (Terra::Entity* ent : back.entities){
            auto& trans = ent->getComponent<Component::Transform>();

            float dist = (x + CAM_WIDTH * shifts[i]) * back.parralax;
            float temp = x * (1.0f - back.parralax);
            trans.position.x = START_POS + dist;

            if (temp > START_POS + WIDTH) START_POS += WIDTH;
            else if (temp < START_POS - WIDTH) START_POS -= WIDTH;

            trans.dirty = true;
            i++;
        }
    }
}

void BackgroundManager::presentBackground(BackgroundType type){
    for (auto& back : backgrounds[type]){
        for (Terra::Entity* ent : back.entities){
            scene->addToRenderer(*ent);
        }
    }

}

void BackgroundManager::removeBackground(BackgroundType type){
    for (auto& back : backgrounds[type]){
        for (Terra::Entity* ent : back.entities)
                scene->removeFromRenderer(*ent);
    }
}

void BackgroundManager::onCollision(const CollisionData& collision){
    
}
