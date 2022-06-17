#pragma once
#include "window.h"
#include "core/input.h"
#include "util/assetPool.h"
#include "scene/scene.h"

//Just the FPS
struct SettingsInfo{

        bool display = true;
        float elapsedTimeBeforeHiding = 0.0f;
        float hideDelay= 0.5f;

        float refreshRate = 0.5f;
        float elapsedTimeBeforeRefresh = 0.0f;
        int FPS = 0;
};

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
        void displaySettingsAndInfo(float delta);
        static Application* instance;
        
        Scene* scene;
        Window* window;
        KeyListener* keyListener;
        MouseListener* mouseListener;
        AssetPool* assetPool;

        SettingsInfo settings;
};