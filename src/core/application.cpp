#include "core/application.h"

#include "renderer/buffer.h"
#include "renderer/shader.h"
#include "renderer/VAO.h"

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
   // delete scene;
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
       scene->render();


        window->finishFrame();
        window->pollEvents();
    }
}