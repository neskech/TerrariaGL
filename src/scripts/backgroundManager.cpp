#include "scripts/backgroundManager.h"
#include "renderer/renderer.h"
#include "core/window.h"

#define DAY_PERIOD 100
#define Y_POS 15.0f
#define TRANSITION_DELAY 0.6f

#define WIDTH CAM_WIDTH

#define SUN_MOON_RADIUS 18.0f
#define SUN_MOON_SCALE 6.0f
#define SUN_MOON_SHIFT_RADIANS 0.6f

#define UNDERGROUND_THRESHOLD -20.0f

static int START_POS = 0.0f;

void BackgroundManager::start(){

    sunAndMoon = &scene->createEntity("Sun and Moon");
    Ref<Texture>& sun = AssetPool::getTexture("../assets/img/sun.png");

    auto& spr = sunAndMoon->addComponent<Component::SpriteRenderer>(SpriteSheet(sun, 1, 1));
    spr.color.a = 0.7f;

    auto& trans = sunAndMoon->getComponent<Component::Transform>();
    trans.setScale(glm::vec2(SUN_MOON_SCALE, SUN_MOON_SCALE));
    scene->addToRenderer(*sunAndMoon);

    currentBackground = BackgroundType::forest;
    backgrounds.reserve(3);

    lastBiome = scene->getWorld()->getBiomeTypeAt(Camera::getPosition().x);

    LightingData& data = Renderer::getLightingData();
    data.dayPeriod = DAY_PERIOD;
    data.currentTime = 0.0f;
    data.day = true;

    initForest();
    initSnow();
    initDesert();
    initUnderGround();

    presentBackground(currentBackground);

    lastCameraPos = Camera::getPosition();
}

void BackgroundManager::update(float timeStep){
    updateSunAndMoon();

    if (transitioning){
        transitionAlphaValue -= (1.0f / TRANSITION_DELAY) * timeStep;
        if (transitionAlphaValue <= 0.0f){
            removeBackground(currentBackground);
            setAlphaValue(currentBackground, 1.0f);
            setZIndex(transitionBackground, -1.0f);
            currentBackground = transitionBackground;
            transitioning = false;
            transitionAlphaValue = 1.0f;
        }
        else
            setAlphaValue(currentBackground, transitionAlphaValue);

        setPosition(transitionBackground, Camera::getPosition());
    }

      Biome b = scene->getWorld()->getBiomeTypeAt(Camera::getPosition().x);
      if (b != lastBiome){
        transitioning = true;
        transitionBackground = biomeToBackground(b);
        lastBiome = b;
        setZIndex(transitionBackground, -2.0f);
        presentBackground(transitionBackground);
     }
     else if (Camera::getPosition().y <= UNDERGROUND_THRESHOLD && lastCameraPos.y > UNDERGROUND_THRESHOLD){
         transitioning = true;
         transitionBackground = BackgroundType::underground;
         setZIndex(transitionBackground, -2.0f);
         presentBackground(transitionBackground);
     }
     else if (Camera::getPosition().y > UNDERGROUND_THRESHOLD && lastCameraPos.y <= UNDERGROUND_THRESHOLD){
         transitioning = true;
         transitionBackground = biomeToBackground(b);
         setZIndex(transitionBackground, -2.0f);
         presentBackground(transitionBackground);
     }

    lastCameraPos = Camera::getPosition();
    elapsedTime += timeStep;
    setPosition(currentBackground, Camera::getPosition());
}

void BackgroundManager::initForest(){
    backgrounds[BackgroundType::forest].reserve(1);

    Ref<Texture> textures[1] = {
        AssetPool::getTexture("../assets/img/forest.png"),
    };

    float p[1] = {
        1.0f,
    };

    setBackground(BackgroundType::forest, textures, p, 1);
}

void BackgroundManager::initSnow(){
    backgrounds[BackgroundType::tundra].reserve(1);

    Ref<Texture> textures[1] = {
        AssetPool::getTexture("../assets/img/snow.png"),
    };

    float p[1] = {
        1.0f,
    };

    setBackground(BackgroundType::tundra, textures, p, 1);
}

void BackgroundManager::initDesert(){
    backgrounds[BackgroundType::desert].reserve(1);

    Ref<Texture> textures[1] = {
        AssetPool::getTexture("../assets/img/desert.png"),
    };

    float p[1] = {
        1.0f,
    };

    setBackground(BackgroundType::desert, textures, p, 1);
}

void BackgroundManager::initUnderGround(){
    backgrounds[BackgroundType::underground].reserve(1);

    Ref<Texture> textures[1] = {
        AssetPool::getTexture("../assets/img/underground.png"),
    };

    float p[1] = {
        1.0f,
    };

    setBackground(BackgroundType::underground, textures, p, 1);
}

void BackgroundManager::setBackground(BackgroundType type, Ref<Texture>* textures, float* p, int length){
    for (int i = 0; i < length; i++){
        backgrounds[type].emplace_back();

        for (int j = 0; j < 3; j++){
            auto& ent = scene->createEntity("Background type " + std::to_string((int)type) + " number #" + std::to_string(j));

            float shift = j == 0 ? -1.0f : (j == 1 ? 0.0f : 1.0f);

            ent.addComponent<Component::SpriteRenderer>(SpriteSheet(textures[i], 1, 1));
            auto& trans = ent.getComponent<Component::Transform>();
            trans.zIndex = -1;
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

void BackgroundManager::setZIndex(BackgroundType type, int z){
    for (auto& back : backgrounds[type]){
        for (Terra::Entity* ent : back.entities){
                auto& trans = ent->getComponent<Component::Transform>();
                trans.zIndex = z;
                trans.dirty = true;
        }
    }
}

void BackgroundManager::setPosition(BackgroundType type, const glm::vec2& pos){
    float shifts[3] = {1.0f, 0.0f, -1.0f};
    for (auto& back : backgrounds[type]){
        int i = 0;
        for (Terra::Entity* ent : back.entities){
            auto& trans = ent->getComponent<Component::Transform>();

            float dist = (pos.x + CAM_WIDTH * shifts[i]) * back.parralax;
            float temp = pos.x * (1.0f - back.parralax);
            trans.position.x = START_POS + dist;

            if (temp > START_POS + WIDTH) START_POS += WIDTH;
            else if (temp < START_POS - WIDTH) START_POS -= WIDTH;

            trans.position.y = pos.y;

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

void BackgroundManager::updateSunAndMoon(){
    LightingData& data = Renderer::getLightingData();
    if (elapsedTime >= DAY_PERIOD){

        auto& spr = sunAndMoon->getComponent<Component::SpriteRenderer>();
        if (day)
            spr.changeTexture(AssetPool::getTexture("../assets/img/moon.png"));
        else 
            spr.changeTexture(AssetPool::getTexture("../assets/img/sun.png"));

        day = !day;

        data.day = !data.day;
        data.currentTime = 0.0f;
        elapsedTime = 0.0f;
    }

    sunAndMoonTheta = -M_PI - SUN_MOON_SHIFT_RADIANS + (elapsedTime / DAY_PERIOD) * (M_PI + 2 * SUN_MOON_SHIFT_RADIANS);
    auto& trans = sunAndMoon->getComponent<Component::Transform>();
    const glm::vec2& p = Camera::getPosition();
    trans.setPos(glm::vec2(SUN_MOON_RADIUS * cos(sunAndMoonTheta) + p.x, SUN_MOON_RADIUS * -sin(sunAndMoonTheta) + p.y));

    data.currentTime = elapsedTime;
}

BackgroundType BackgroundManager::biomeToBackground(Biome b){
    switch (b){
        case Biome::Forest:
            return BackgroundType::forest;
        case Biome::Snow:
            return BackgroundType::tundra;
        case Biome::Desert:
            return BackgroundType::desert;
        default:
            assert(false);
    }
}

void BackgroundManager::onCollision(const CollisionData& collision){
    
}

void BackgroundManager::onDestroy(){

}
