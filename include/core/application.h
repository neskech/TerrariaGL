#pragma once
#include "window.h"
#include "core/input.h"
#include "util/assetpool.h"

class Application{

    public:
        Application(uint32_t windowWidth, uint32_t windowHeight);
        ~Application();
        Application(const Application& other) = delete;
        Application(Application&& other) = delete;

        bool init();
        void run();

        inline Application* getInstance(){ return instance; }
        inline Window* getWindow(){ return window; }

    private:
        static Application* instance;
        
        Window* window;
        KeyListener* keyListener;
        MouseListener* mouseListener;
        AssetPool* assetPool;
};