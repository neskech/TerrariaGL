#include "core/application.h"

#include "renderer/buffer.h"
#include "renderer/shader.h"
#include "renderer/VAO.h"

#include "renderer/fontRenderer.h"

Application* Application::instance = nullptr;

Application::Application(uint32_t windowWidth, uint32_t windowHeight){
    if (instance != nullptr)
        assert(false);

    instance = this;
    window = new Window(windowWidth, windowHeight);

    keyListener = new KeyListener();
    mouseListener = new MouseListener(window);
    assetPool = new AssetPool();
}

Application::~Application(){
    delete scene;
    delete assetPool;
    delete keyListener;
    delete mouseListener;
    delete window;
}

bool Application::init(){
    if (!window->init()){
        std::cerr << "ERROR: Application --> Window initialization failed!\n";
        return false;
    }

    scene = new Scene();
    scene->init();
    
    return true;
}

void Application::run(){
    double delta;
    double end = 0.0;
    double now;


    while (!window->windowClosing()){
        now = glfwGetTime();
        float delta = now - end;
        end = now;

        window->update();

        scene->update(delta);
        displaySettingsAndInfo(delta);
        scene->render();

        window->finishFrame();
        window->pollEvents();
    }
}

void Application::displaySettingsAndInfo(float delta){

    if (settings.elapsedTimeBeforeRefresh >= settings.refreshRate){
        settings.FPS = (int) (1.0f / delta);
        settings.elapsedTimeBeforeRefresh = 0.0f;
    }

    if (KeyListener::isKeyPressed(GLFW_KEY_TAB) && settings.elapsedTimeBeforeHiding >= settings.hideDelay){
        settings.elapsedTimeBeforeHiding = 0.0f;
        settings.display = !settings.display;
    }

    if (settings.display){
        const glm::vec2& p = Camera::getPosition();
        glm::vec2 pos(p.x + CAM_WIDTH / 2.0f - 9.0f, p.y + CAM_HEIGHT / 2.0f - 1.0f);

        FontRenderer::drawText(std::string("FPS ") + std::to_string(settings.FPS), pos, 
        1.0f, 0.1f, glm::vec3(1.0f, 1.0, 1.0f));
    }

    settings.elapsedTimeBeforeRefresh += delta;
    settings.elapsedTimeBeforeHiding += delta;
}

