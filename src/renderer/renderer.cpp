#include "renderer/renderer.h"
#include "core/window.h"

#define TILE_MAP_Z_INDEX 1.0

Renderer* Renderer::instance = nullptr;

Renderer::Renderer(): frameBufferVBO(BufferType::VBO)
{
    if (instance != nullptr)
        assert(false);
    instance = this;

    lightingData.dayColor = glm::vec3(0.20f, 0.25f, 0.99f); //ambient
    lightingData.nightColor = glm::vec3(0.95f, 0.70f, 0.50f); //ambient
    lightingData.interpolationThreshold = 0.1f;
    
    lightingData.maxAmbientStrengthDay = 0.8f;
    lightingData.minAmbientStrengthDay = 0.5f;

    lightingData.maxAmbientStrengthNight = 0.4f;
    lightingData.minAmbientStrengthNight = 0.1f;

}

Renderer::~Renderer(){
    //reverse order destruction
    delete fontRenderer;
    delete debugRenderer;
    delete tileMapRenderer;

}

void Renderer::init(World* world){
    bool success = frameBuffer.create();
    assert(success);

    frameBufferShader = AssetPool::getShader("../assets/shaders/frameShader.glsl");

    float verticies[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    frameBufferVAO.bind();

    frameBufferVBO.bind();
    frameBufferVBO.addData(verticies, 24, 0, GL_STATIC_DRAW);

    //positions
    frameBufferVAO.addAtribute(2, GL_FLOAT, sizeof(float), 4 * sizeof(float));
    //uvs
    frameBufferVAO.addAtribute(2, GL_FLOAT, sizeof(float), 4 * sizeof(float));

    frameBufferVBO.unBind();
    frameBufferVAO.unBind();

    //must be called after window initialization
    spriteRenderers.reserve(MAX_INSTANCE_RENDERERS);

    tileMapRenderer = new TileMapRenderer(world);
    tileMapRenderer->init();

    debugRenderer = new DebugRenderer();
    debugRenderer->init();

    fontRenderer = new FontRenderer();
    fontRenderer->init();
}

static glm::vec3 vectorInterp(const glm::vec3& v1, const glm::vec3& v2, float t){
    glm::vec3 f;
    f.x = v1.x + t * (v2.x - v1.x);
    f.y = v1.y + t * (v2.y - v1.y);
    f.z = v1.z + t * (v2.z - v1.z);
    return f;
}

void Renderer::render(){
    frameBuffer.bind();

    frameBuffer.enableGLSettings();
    frameBuffer.clearColor(0.9f, 0.95f, 0.9f, 1.0f);
    frameBuffer.clearType(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    renderScene();

    frameBuffer.unBind();
    glClearColor(0.9f, 0.95f, 0.9f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    frameBufferShader->activate(); 

    glm::vec3 ambientColor;
    float ambientStrength;
    float dayProp = lightingData.currentTime / lightingData.dayPeriod;
    if (1.0f - dayProp <= lightingData.interpolationThreshold){
        float t = (1.0f - dayProp) / lightingData.interpolationThreshold;
        t = 1.0f - t;
        if (lightingData.day){
             ambientStrength = lightingData.minAmbientStrengthDay + t * (lightingData.minAmbientStrengthNight - lightingData.minAmbientStrengthDay);
             ambientColor = vectorInterp(lightingData.nightColor, lightingData.dayColor, t);
        }
        else{
             ambientStrength = lightingData.minAmbientStrengthNight + t * (lightingData.minAmbientStrengthDay - lightingData.minAmbientStrengthNight);
             ambientColor = vectorInterp(lightingData.dayColor, lightingData.nightColor, t);
        }

    }

    else{
        float x = (dayProp - 0.5f) * 2.0f;
        if (lightingData.day){

            ambientStrength = MAX(-(x * x) + lightingData.maxAmbientStrengthDay, lightingData.minAmbientStrengthDay);
            ambientColor = lightingData.nightColor;
        }
        else{
            ambientStrength = MAX(-(x * x) + lightingData.maxAmbientStrengthNight, lightingData.minAmbientStrengthNight);
            ambientColor = lightingData.dayColor;
        }

    }

    frameBufferShader->setFloat("ambience", ambientStrength);
    frameBufferShader->setVec3("ambientColor", ambientColor);
    frameBufferShader->setVec3("ambientColor", ambientColor);

    const glm::vec2& cam = Camera::getPosition();

    const int numTorches = MIN(8 - lightingData.playerLight, lightingData.torches.size());
    frameBufferShader->setFloat("numTorches", numTorches + lightingData.playerLight);

    if (lightingData.playerLight){
        glm::vec2 p;
        p.x = (lightingData.playerTransform->position.x - cam.x + CAM_WIDTH / 2.0f) / CAM_WIDTH * Window::getWidth();
        p.y = (lightingData.playerTransform->position.y - cam.y + CAM_HEIGHT / 2.0f) / CAM_HEIGHT * Window::getHeight();
        frameBufferShader->setVec2("torchLocations[0]", p);
    }


    for (int i = 0; i < numTorches; i++){ 
        glm::vec2 p;
        p.x = (lightingData.torches[i].x - cam.x + CAM_WIDTH / 2.0f) / CAM_WIDTH * Window::getWidth();
        p.y = (lightingData.torches[i].y - cam.y + CAM_HEIGHT / 2.0f) / CAM_HEIGHT * Window::getHeight();
        frameBufferShader->setVec2(std::string("torchLocations[") + std::to_string(i + lightingData.playerLight) + std::string("]"), p);
    }

    frameBufferVAO.bind();
    glDisable(GL_DEPTH_TEST);
    glBindTexture(GL_TEXTURE_2D, frameBuffer.textureColorAttachment());
    glDrawArrays(GL_TRIANGLES, 0, 6);  
    glBindTexture(GL_TEXTURE_2D, 0);
    frameBufferVAO.unBind();
    frameBufferShader->deActivate();

}

void Renderer::renderScene(){
    debugRenderer->render();

    bool tileMapRendered = false;
    for (auto& rend : spriteRenderers){
         if (rend->getZIndex() >= TILE_MAP_Z_INDEX && !tileMapRendered){
             tileMapRenderer->render();
             tileMapRendered = true;
         }
         for (int i = 0; i < rend->numSprites; i++){
             Component::Tag& tag = rend->entities[i]->getComponent<Component::Tag>();
             Component::Transform& trans = rend->entities[i]->getComponent<Component::Transform>();
         }
         rend->render();
    }

    if (!tileMapRendered){
         tileMapRenderer->render();
    }
    fontRenderer->render();
}

void Renderer::submit(Terra::Entity& ent){

    auto& spr = ent.getComponent<Component::SpriteRenderer>();
    auto& trans = ent.getComponent<Component::Transform>();

    uint32_t numSprites = 0;
    for (auto& rend : spriteRenderers){
        numSprites += rend->getNumSprites();
        if (rend->hasRoomSprites() && (rend->hasRoomTextures() || rend->containsTexture(spr))
        && rend->getZIndex() == trans.zIndex){
            rend->addEntity(ent);
            return;
        }
    }

    assert(numSprites < MAX_INSTANCES);
    spriteRenderers.push_back(std::make_unique<SpriteRenderer>(*this, trans.zIndex));
    spriteRenderers.back()->init();
    spriteRenderers.back()->addEntity(ent);
    assert(spriteRenderers.size() < MAX_INSTANCE_RENDERERS);
    
    std::sort(spriteRenderers.begin(), spriteRenderers.begin() + spriteRenderers.size(), 
        [](const Scoped<SpriteRenderer>& spr1, const Scoped<SpriteRenderer>& spr2){
            return spr1->getZIndex() < spr2->getZIndex();
        }
    );

}

void Renderer::remove(Terra::Entity& ent){
    //assert that the renderer actually contains the given entity
     for (auto& rend : spriteRenderers){
         if (rend->containsEntity(ent)){ 
            rend->removeEntity(ent);
            return;
         }
    }

    assert(false);
}

