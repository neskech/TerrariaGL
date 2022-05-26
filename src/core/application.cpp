#include "core/application.h"
#include <cassert>

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
}

void Application::run(){
    double delta;
    double end = 0.0;
    double now;

    while (!window->windowClosing()){
        now = glfwGetTime();
        std::cout << "Delta time : " << (now - end) / 1e9 << "\n";
        end = now;

        window->update();
        window->finishFrame();
        window->pollEvents();
    }
}